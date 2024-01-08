#pragma once

#include <thread>
#include <functional>


/*-------------------------------------------
			    ThreadManager

			Thread를 관리하는 클래스
-------------------------------------------*/
class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void	Launch(function<void(void)> callback);
	void	Join();

	static void InitTLS();
	static void DestroyTLS();

	static void DoGlobalQueueWork();

private:
	Mutex			_lock;
	vector<thread>	_threads;
};

