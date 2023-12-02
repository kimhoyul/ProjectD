#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

CoreGlobal Core; // CoreGlobal의 생성자가 호출되면서 GThreadManager가 생성된다

void ThreadMain()
{
	while (true)
	{
		cout << "hello! I am thread...." << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadMain);
	}
	
	GThreadManager->Join();
}