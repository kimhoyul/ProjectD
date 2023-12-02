#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

thread_local int32 LThreadId = 0; // thread_local : thread 별로 각각의 변수를 가질 수 있게 해줌

void ThreadMain(int32 threadId)
{
	LThreadId = threadId; // thread_local 변수에 threadId를 넣어줌

	while (true)
	{
		std::cout << "Thread " << threadId << " is running" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 대기
	}
}

int main()
{
	vector<thread> threads;

	for (int32 i = 0; i < 10; ++i)
	{
		int32 threadId = i + 1;
		threads.push_back(thread(ThreadMain, threadId));
	}

	for (thread& t : threads)
	{
		t.join();
	}
}