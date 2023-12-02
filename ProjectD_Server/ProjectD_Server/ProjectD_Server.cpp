#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

atomic<bool> ready;
int32 value;

void Producer()
{
	value = 10;

	ready.store(true, memory_order::memory_order_release);
	// ---------------------------- 이 아래로 코드 재배치 X
}

void Consumer()
{
	// ---------------------------- 이 위로 코드 재배치 X
	while (ready.load(memory_order::memory_order_acquire) == false)
	;

	cout << "value : " << value << endl;
}

int main()
{
	ready = false;
	value = 0;
	thread t1(Producer);
	thread t2(Consumer);
	t1.join();
	t2.join();

	// Memory Model 
	// 1. Sequential Consistency : seq_cst = 가장 엄격하게 동기화 = 직관적
	// 가시성, 재배치 해결
	
	// 2. Acquire-Release : 
	// release 명령 이전의 메모리 명령들이, release 명령 이후의 메모리 명령들보다 먼저 수행되지 않는다.
	// acquire 명령 이후의 메모리 명령들이, acquire 명령 이전의 메모리 명령들보다 먼저 수행되지 않는다.
	// 가시성 보장, 재배치 부분적 해결
	
	// 3. Relaxed : relaxed = 가장 느슨하게 동기화 = 직관적이지 않음
	// 코드 재배치 멋대로, 가시성도 해결 X

	// 인텔, AMD 일관성 보장 O
	// ARM, PowerPC 일관성 보장 X
}