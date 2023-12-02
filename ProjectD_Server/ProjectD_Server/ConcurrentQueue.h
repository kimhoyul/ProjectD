#pragma once

#include <mutex>

template<typename T>
class LockQueue
{
public:
	LockQueue() {}

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex); // lock_guard: mutex를 lock하고 unlock하는 것을 자동으로 해줌
		_queue.push(value); // std::move : lvalue를 rvalue로 변환
		_condVar.notify_one(); // 조건변수를 사용하여 대기중인 스레드를 깨움
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex); // lock_guard: mutex를 lock하고 unlock하는 것을 자동으로 해줌
		if (_queue.empty())
			return false; // 스택이 비어있으면 pop 실패로 false 리턴

		value = std::move(_queue.front()); // queue의 front를 value에 저장
		_queue.pop(); // queue의 front를 pop
		return true; // 성공적으로 pop을 했으면 true 리턴
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex); // unique_lock : mutex를 lock하고 unlock하는 것을 자동으로 해줌
		_condVar.wait(lock, [this]() { return _queue.empty() == false; }); // 조건변수를 사용하여 스택이 비어있지 않을 때까지 대기
		value = std::move(_queue.front()); // 큐의 front를 value에 저장
		_queue.pop(); // 큐의 front를 pop
	}

private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};