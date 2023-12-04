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
	void WriteLcok(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

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
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.ReadLock(name); }
	~ReadLockGuard() { _lock.ReadUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLcok(name); }
	~WriteLockGuard() { _lock.WriteUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;
};