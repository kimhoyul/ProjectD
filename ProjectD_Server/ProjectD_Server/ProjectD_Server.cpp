#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>

mutex m;
queue<int32> q;
HANDLE handle;

void Producer()
{
	while (true)
	{
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		::SetEvent(handle);
		//this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE); // handle이 0이면 무한대기, 0이 아니면 handle이 1이 될 때까지 대기

		unique_lock<mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << q.size() << endl;
		}
	}
}

int main()
{
	// 커널 오브젝트 : 커널이 관리하는 자원
	// UsageCount : 커널 오브젝트를 사용하는 스레드의 개수
	// Signal : 커널 오브젝트를 사용하는 스레드가 없다면 0, 있다면 1
	// bManualReset : true면 Signal이 1이 되면 0으로 바뀌지 않음, false면 Signal이 1이 되면 0으로 바뀜
	handle = ::CreateEvent(NULL/*보안속성*/, FALSE/*bManualReset*/, FALSE/*bInitialState*/, NULL);

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}