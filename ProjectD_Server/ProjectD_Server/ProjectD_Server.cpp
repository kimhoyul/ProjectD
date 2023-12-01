#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>

// atomic : 원자적 연산을 보장해주는 클래스
// 원자적 연산 : 연산이 완전히 끝날 때까지 다른 스레드가 접근하지 못하도록 보장하는 연산
atomic<int32> sum = 0; // global variable

void Add()
{
	for (int i = 0; i < 1'000'000; ++i)
	{
		sum.fetch_add(1); // sum = sum + 1
	}
}

void Sub()
{
	for (int i = 0; i < 1'000'000; ++i)
	{
		sum.fetch_sub(1); // sum = sum - 1
	}
}

int main()
{
	Add();
	Sub();
	
	cout << sum << endl;

	std::thread t1(Add);
	std::thread t2(Sub);
	t1.join();
	t2.join();

	cout << sum << endl;
}
