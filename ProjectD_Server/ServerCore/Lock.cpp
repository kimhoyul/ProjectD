#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

/*-------------------------------------------
		   Reader-Writer Spin Lock

   [WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : Write Flag (ExclusiveLock Owner TheradId)
R : Read Lock (SharedLock Count)
-------------------------------------------*/
void Lock::WriteLcok(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// 동일한 스레드가 소유하고 있다면 무조건 성공
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId) // 스레드 아이디가 같다면
	{
		_writeCount++; // 재귀적 호출의 추적을 위한 카운트
		return;
	}
	
	// 아무도 소유 및 공유하고 있지 않을 때, 경합해서 소유권을 얻는다
	// _lockFlag == EMPTY_FLAG 일때만 소유권을 얻는다
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK); // 상위 16비트에 스레드 아이디를 넣기 위해 16비트 왼쪽으로 시프트
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) // MAX_SPIN_COUNT번 돌면서 경합
		{
			uint32 expected = EMPTY_FLAG; 
			if (_lockFlag.compare_exchange_strong(OUT expected, desired)) // _lockFlag == EMPTY_FLAG 일때만 소유권을 얻는다, 소유권을 얻으면 스레드 아이디를 넣어준다.
			{
				_writeCount++; // 재귀적 호출의 추적을 위한 카운트
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK) // 경합에 실패한 시간이 ACQUIRE_TIMEOUT_TICK 를 초과하면
			CRASH("LOCK_TIMEOUT"); // 크래시

		this_thread::yield(); // 스레드 양보
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	// ReadLock을 다 풀기 전에는 WriteLock을 풀 수 없다
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0) // ReadLock이 걸려있다면
		CRASH("INVALID_UNLOCK_ORDER"); // 크래시

	const int32 lockCount = --_writeCount;
	if (lockCount == 0) // 재귀적 호출의 추적을 위한 카운트가 0이면
		_lockFlag.store(EMPTY_FLAG); // 소유권을 반납한다
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// 동일한 스레드가 소유하고 있다면 무조건 성공
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId) // 스레드 아이디가 같다면
	{
		_lockFlag.fetch_add(1); // ReadLock 카운트를 1 증가시킨다
		return;
	}

	// 아무도 소유하고 있지 않을 때, 경합해서 소유권을 얻는다
	// _lockFlag == EMPTY_FLAG 일때만 소유권을 얻는다
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) // MAX_SPIN_COUNT번 돌면서 경합
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK); // 기존의 ReadLock 카운트를 가져온다
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1)) // _lockFlag == Writer Lock 이 아닐때만 소유권을 얻는다, 소유권을 얻으면 readCount를 1 증가시킨다.
				return;
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK) // 경합에 실패한 시간이 ACQUIRE_TIMEOUT_TICK 를 초과하면
			CRASH("LOCK_TIMEOUT"); // 크래시

		this_thread::yield(); // 스레드 양보
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0) // ReadLock 카운트를 1 감소시키고, ReadLock 카운트가 0이면
		CRASH("INVALID_UNLOCK_ORDER"); // 크래시
}
