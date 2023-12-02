#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

/*-------------------------------------------
				ThreadManager

			Thread�� �����ϴ� Ŭ����
-------------------------------------------*/
ThreadManager::ThreadManager()
{
	//Main Thread �ʱ�ȭ
	InitTLS(); // Main Thread �� �׻� LThreadId = 1 �� ���´�
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock); //lock�� �ɾ��ش�.

	_threads.push_back(thread([=]() // thread�� �����Ѵ�.
		{
			InitTLS(); // thread�� ������ ������ TLS�� �ʱ�ȭ���ش�.
			callback(); // callback �Լ��� �����Ѵ�.
			DestroyTLS(); // callback �Լ��� ����Ǹ� TLS�� �������ش�.
		}
	));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable()) // thread�� joinable�� �������� Ȯ��
			t.join();
	}
	_threads.clear(); // thread�� ��� join�� �Ŀ��� vector�� ����ش�.
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1); // LThreadId �� ���� SThreadId ���� �ְ�, SThreadId ���� 1 ������Ų��.
}

void ThreadManager::DestroyTLS()
{
	
}


