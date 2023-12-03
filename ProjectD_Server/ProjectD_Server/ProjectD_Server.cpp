#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

class TestLock
{
	USE_LOCK; // USE_LOCK 매크로를 사용하면 _lock 멤버 변수가 생성된다

public:
	int32 TestRead()
	{
		READ_LOCK; // READ_LOCK 매크로를 사용하면 readLockGuard_0 멤버 변수가 생성된다

		if (_queue.empty())
			return -1;

		return _queue.front();
	}

	void TestPush()
	{
		WRITE_LOCK; // WRITE_LOCK 매크로를 사용하면 writeLockGuard_0 멤버 변수가 생성된다
		_queue.push(rand() % 100);
	}

	void TestPop()
	{
		WRITE_LOCK; // WRITE_LOCK 매크로를 사용하면 writeLockGuard_0 멤버 변수가 생성된다

		if (_queue.empty() == false)
			_queue.pop();
	}
private:
	queue<int32> _queue;
};

TestLock testLock;

void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}

void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();
		cout << value << endl;
		this_thread::sleep_for(1ms);
	}
}

int main()
{
	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}
	
	GThreadManager->Join();
}