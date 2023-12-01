#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>

class SpinLock
{
public:
	void lock()
	{
		// CAS (Compare And Swap)
		// _locked.compare_exchange_strong (expected, desired)
		// expected == _locked 이면 _locked = desired
		// expected != _locked 이면 expected = _locked

		// 의사 코드를 유심히 봐야함 
		// expected != _locked 경우에만!!!! expected = _locked를 해줌

		bool expected = false;
		bool desired = true;

		while (_locked.compare_exchange_strong(expected, desired) == false)
		{
			expected = false;

			//this_thread::sleep_for(std::chrono::milliseconds(100)); // C++ 11
			//this_thread::sleep_for(100ms); // C++ 14 
			this_thread::yield(); // 커널에게 제어권을 넘겨줌 == this_thread::sleep_for(0ms)
		}
	}

	void unlock()
	{
		_locked.store(false);
	}

private:
	atomic<bool> _locked = false;
};

int32 sum = 0;
mutex m;

SpinLock spinLock;

void Add()
{
	for (int i = 0; i < 100'000; ++i)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum++;
	}
}

void Sub()
{
	for (int i = 0; i < 100'000; ++i)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum--;
	}
}

int main()
{
	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}