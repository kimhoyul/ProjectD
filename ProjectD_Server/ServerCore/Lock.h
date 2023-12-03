#pragma once
#include "Types.h"

/*-------------------------------------------
	       Reader-Writer Spin Lock

   [WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : Write Flag (ExclusiveLock Owner TheradId)
R : Read Lock (SharedLock Count)
-------------------------------------------*/
class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10'000, // 대기 시간
		MAX_SPIN_COUNT = 5'000, // 5,000번 스핀하고 넘어가면 스레드를 양보한다.
		WRITE_THREAD_MASK = 0xFFFF'0000, // 상위 16비트
		READ_COUNT_MASK = 0x0000'FFFF, // 하위 16비트
		EMPTY_FLAG = 0x0000'0000, // 0
	};

public:
	void WriteLcok();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private:
    Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;
};

/*-------------------------------------------
				LockGuards
-------------------------------------------*/
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock) { _lock.ReadLock(); }
	~ReadLockGuard() { _lock.ReadUnlock(); }
private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { _lock.WriteLcok(); }
	~WriteLockGuard() { _lock.WriteUnlock(); }
private:
	Lock& _lock;
};