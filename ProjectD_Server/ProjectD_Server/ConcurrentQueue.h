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
		lock_guard<mutex> lock(_mutex); // lock_guard: mutex�� lock�ϰ� unlock�ϴ� ���� �ڵ����� ����
		_queue.push(value); // std::move : lvalue�� rvalue�� ��ȯ
		_condVar.notify_one(); // ���Ǻ����� ����Ͽ� ������� �����带 ����
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex); // lock_guard: mutex�� lock�ϰ� unlock�ϴ� ���� �ڵ����� ����
		if (_queue.empty())
			return false; // ������ ��������� pop ���з� false ����

		value = std::move(_queue.front()); // queue�� front�� value�� ����
		_queue.pop(); // queue�� front�� pop
		return true; // ���������� pop�� ������ true ����
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex); // unique_lock : mutex�� lock�ϰ� unlock�ϴ� ���� �ڵ����� ����
		_condVar.wait(lock, [this]() { return _queue.empty() == false; }); // ���Ǻ����� ����Ͽ� ������ ������� ���� ������ ���
		value = std::move(_queue.front()); // ť�� front�� value�� ����
		_queue.pop(); // ť�� front�� pop
	}

private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};