#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <future>

int64 Calcualte()
{
	int64 sum = 0;

	for (int32 i = 0; i < 1'000'000; ++i)
	{
		sum += i;
	}

	return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("Secret Message");
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}

int main()
{
	// 동기(synchronous) 실행
	//int64 sum = Calcualte();
	//cout << sum << endl;

	//std::future : 비동기(asynchronous) 실행
	{
		// 1) deferred : 지연된 실행
		// 2) async : 비동기 실행
		// 3) deferred | async : 지연된 실행 or 비동기 실행 알아서 결정
		std::future<int64> future = std::async(std::launch::async, Calcualte);

		std::future_status status = future.wait_for(1ms); // 1ms 동안 기다린다.
		if (status == future_status::ready) // 만약 1ms 동안 기다렸는데 결과가 나왔다면
		{
			int64 sum = future.get(); // 결과를 가져온다.
		}

		int64 sum = future.get(); // 결과를 가져올 때까지 기다린다.

		/*class Knight
		{
		public:
			int64 GetHP() { return 100;}
		};

		Knight knight;
		std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHP, knight);*/
	}

	// std::promise : 비동기(asynchronous) 실행
	{
		std::promise<string> promise; // 약속 개체생성
		std::future<string> future = promise.get_future(); // 약속 개체로부터 미래의 결과를 가져온다.

		thread t(PromiseWorker, std::move(promise)); // 다른 쓰레드에서 약속 개체기다림

		string message = future.get(); // 미래의 결과로부터 결과를 가져온다.
		cout << message << endl;

		t.join();
	}

	// std::packaged_task : 비동기(asynchronous) 실행
	{
		std::packaged_task<int64/*걸어줄 함수의 아웃풋 타입*/(void/*걸어줄 함수의 인풋타입*/)> task(Calcualte); // 작업 개체 생성
		std::future<int64> future = task.get_future(); // 작업 개체로부터 미래의 결과를 가져온다.

		thread t(TaskWorker, std::move(task)); // 다른 쓰레드에서 작업 개체 실행

		int64 sum = future.get(); // 미래의 결과로부터 결과를 가져온다.
		cout << sum << endl;

		t.join();
	}
}