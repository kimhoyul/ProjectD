#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

// 가시성과 코드 재배치가 문제가 됨

// 가시성: 코어가 여러개인데, 코어마다 캐시가 있음
// 캐시가 있으면 캐시에 복사해서 쓰는데, 캐시가 다르면 캐시에 있는 값이 다름

// 코드 재배치: 컴파일러 또는 CPU가 최적화를 위해 코드를 재배치함
// 함수를 기계어로 변환과정에서 단일스레드 기준 함수의 결과값이 같다면, 순서를 바꿔도 상관없다고 생각함
// 컴파일러가 재배치를 하면, 내가 원하는 순서대로 실행이 안됨

int32 x = 0;
int32 y = 0;
int32 r1 = 0;
int32 r2 = 0;
volatile bool ready = false;

void Thread_1()
{
	while (ready == false) ; //wait until ready
	y = 1; //store y
	r1 = x; //load x
}

void Thread_2()
{
	while (ready == false); //wait until ready
	x = 1; //store x
	r2 = y; //load y
}

int main()
{
	int32 count = 0;

	while (true)
	{
		ready = false;
		count++;

		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
		{
			cout << count << "번째에서 발견" << endl;
			break;
		}
	}
}