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
	// ������ �����尡 �����ϰ� �ִٸ� ������ ����
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId) // ������ ���̵� ���ٸ�
	{
		_writeCount++; // ����� ȣ���� ������ ���� ī��Ʈ
		return;
	}
	
	// �ƹ��� ���� �� �����ϰ� ���� ���� ��, �����ؼ� �������� ��´�
	// _lockFlag == EMPTY_FLAG �϶��� �������� ��´�
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK); // ���� 16��Ʈ�� ������ ���̵� �ֱ� ���� 16��Ʈ �������� ����Ʈ
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) // MAX_SPIN_COUNT�� ���鼭 ����
		{
			uint32 expected = EMPTY_FLAG; 
			if (_lockFlag.compare_exchange_strong(OUT expected, desired)) // _lockFlag == EMPTY_FLAG �϶��� �������� ��´�, �������� ������ ������ ���̵� �־��ش�.
			{
				_writeCount++; // ����� ȣ���� ������ ���� ī��Ʈ
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK) // ���տ� ������ �ð��� ACQUIRE_TIMEOUT_TICK �� �ʰ��ϸ�
			CRASH("LOCK_TIMEOUT"); // ũ����

		this_thread::yield(); // ������ �纸
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	// ReadLock�� �� Ǯ�� ������ WriteLock�� Ǯ �� ����
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0) // ReadLock�� �ɷ��ִٸ�
		CRASH("INVALID_UNLOCK_ORDER"); // ũ����

	const int32 lockCount = --_writeCount;
	if (lockCount == 0) // ����� ȣ���� ������ ���� ī��Ʈ�� 0�̸�
		_lockFlag.store(EMPTY_FLAG); // �������� �ݳ��Ѵ�
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// ������ �����尡 �����ϰ� �ִٸ� ������ ����
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId) // ������ ���̵� ���ٸ�
	{
		_lockFlag.fetch_add(1); // ReadLock ī��Ʈ�� 1 ������Ų��
		return;
	}

	// �ƹ��� �����ϰ� ���� ���� ��, �����ؼ� �������� ��´�
	// _lockFlag == EMPTY_FLAG �϶��� �������� ��´�
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) // MAX_SPIN_COUNT�� ���鼭 ����
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK); // ������ ReadLock ī��Ʈ�� �����´�
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1)) // _lockFlag == Writer Lock �� �ƴҶ��� �������� ��´�, �������� ������ readCount�� 1 ������Ų��.
				return;
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK) // ���տ� ������ �ð��� ACQUIRE_TIMEOUT_TICK �� �ʰ��ϸ�
			CRASH("LOCK_TIMEOUT"); // ũ����

		this_thread::yield(); // ������ �纸
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0) // ReadLock ī��Ʈ�� 1 ���ҽ�Ű��, ReadLock ī��Ʈ�� 0�̸�
		CRASH("INVALID_UNLOCK_ORDER"); // ũ����
}
