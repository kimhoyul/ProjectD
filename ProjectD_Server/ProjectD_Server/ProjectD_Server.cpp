#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>

mutex m;
queue<int32> q;

// condition_variable : 스레드가 특정 조건을 만족할 때까지 대기하고 있다가 조건이 만족되면 깨어나는 기능
// UserLevel Object : 커널이 관리하지 않는 자원
condition_variable cv;

void Producer()
{
	while (true)
	{
		{
			unique_lock<mutex> lock(m); // 1) Lock을 걸고
			q.push(100); // 2) 공유 변수 값을 수정
		} // 3) Lock을 풀고
		cv.notify_one(); // 4) 조건 변수 통해 다른 쓰레드에게 통지해 대기중인 스레드가 있다면 한개만 깨움 

		//this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	while (true)
	{
		unique_lock<mutex> lock(m); // 1) Lock을 걸고
		cv.wait(lock, []() { return q.empty() == false; }); 
		// 2) Lock이 걸려있는지 확인후 안잡혀있으면 다시 잡고, 
		// 조건만족 O = 확인후 만족하면 빠져 나와서 이어서 코드 진행
		// 조건만족 X = Lock을 풀고 대기 상태로 들어감
		
		{
			int32 data = q.front();
			q.pop();
			cout << q.size() << endl;
		}
	}
}

int main()
{
	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();
}