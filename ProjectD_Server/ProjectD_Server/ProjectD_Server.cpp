#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>

vector<int32> v;

// mutex : mutual exclusion (상호 배제)
mutex m;

// RAII patten : Resource Acquisition Is Initialization (자원 획득은 초기화 과정에서)
// 이 패턴의 핵심 아이디어는 객체의 생명주기를 통해 자원의 관리를 자동화하는 것
// 즉, 객체가 생성될 때 자원을 획득(acquisition)하고, 객체가 소멸될 때 자원을 해제(release)
// 이 패턴을 사용하면 자원을 획득한 후, 예외가 발생하더라도 자원을 해제할 수 있음

template<typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}

	~LockGuard()
	{
		_mutex->unlock();
	}

private:
	T* _mutex;
};

void push()
{
	for (int32 i = 0; i < 10'000; ++i)
	{
		std::lock_guard<std::mutex> lockGuard(m);
		v.push_back(i);
	}
}

int main()
{
	std::thread t1(push);
	std::thread t2(push);

	t1.join();
	t2.join();

	cout << v.size() << endl;
}