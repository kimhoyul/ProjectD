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
		ACQUIRE_TIMEOUT_TICK = 10'000, // ��� �ð�
		MAX_SPIN_COUNT = 5'000, // 5,000�� �����ϰ� �Ѿ�� �����带 �纸�Ѵ�.
		WRITE_THREAD_MASK = 0xFFFF'0000, // ���� 16��Ʈ
		READ_COUNT_MASK = 0x0000'FFFF, // ���� 16��Ʈ
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