#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

/*-------------------------------------------
				ThreadManager

			Thread를 관리하는 클래스
-------------------------------------------*/
ThreadManager::ThreadManager()
{
	//Main Thread 초기화
	InitTLS(); // Main Thread 가 항상 LThreadId = 1 을 갖는다
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock); //lock을 걸어준다.

	_threads.push_back(thread([=]() // thread를 생성한다.
		{
			InitTLS(); // thread를 생성할 때마다 TLS를 초기화해준다.
			callback(); // callback 함수를 실행한다.
			DestroyTLS(); // callback 함수가 종료되면 TLS를 해제해준다.
		}
	));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable()) // thread가 joinable한 상태인지 확인
			t.join();
	}
	_threads.clear(); // thread를 모두 join한 후에는 vector를 비워준다.
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1); // LThreadId 에 현재 SThreadId 값을 넣고, SThreadId 값을 1 증가시킨다.
}

void ThreadManager::DestroyTLS()
{
	
}


