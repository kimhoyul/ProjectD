#pragma once

#include <mutex>

template<typename T>
class LockStack // ������ ��Ƽ�����忡�� ����� �� �ֵ��� lock�� �ɾ���
{
public:
	LockStack() {}
	
	LockStack(const LockStack&) = delete; // ���� ������ ���� : LockStack lockStack2(lockStack) X
	LockStack& operator=(const LockStack&) = delete; // ���� ���۷����� ���� : lockStack = lockStack2 X 

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex); // lock_guard: mutex�� lock�ϰ� unlock�ϴ� ���� �ڵ����� ����
		_stack.push(std::move(value)); // std::move : lvalue�� rvalue�� ��ȯ
		_condVar.notify_one(); // ���Ǻ����� ����Ͽ� ������� �����带 ����
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex); // lock_guard: mutex�� lock�ϰ� unlock�ϴ� ���� �ڵ����� ����
		if (_stack.empty()) 
			return false; // ������ ��������� pop ���з� false ����

		value = std::move(_stack.top()); // ������ top�� value�� ����
		_stack.pop(); // ������ top�� pop
		return true; // ���������� pop�� ������ true ����
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex); // unique_lock : mutex�� lock�ϰ� unlock�ϴ� ���� �ڵ����� ����
		_condVar.wait(lock, [this]() { return _stack.empty() == false; }); // ���Ǻ����� ����Ͽ� ������ ������� ���� ������ ���
		value = std::move(_stack.top()); // ������ top�� value�� ����
		_stack.pop(); // ������ top�� pop
	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};