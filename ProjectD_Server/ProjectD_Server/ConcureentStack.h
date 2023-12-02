#pragma once

#include <mutex>

template<typename T>
class LockStack // 스택을 멀티스레드에서 사용할 수 있도록 lock을 걸어줌
{
public:
	LockStack() {}
	
	LockStack(const LockStack&) = delete; // 복사 생성자 삭제 : LockStack lockStack2(lockStack) X
	LockStack& operator=(const LockStack&) = delete; // 복사 오퍼레이터 삭제 : lockStack = lockStack2 X 

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex); // lock_guard: mutex를 lock하고 unlock하는 것을 자동으로 해줌
		_stack.push(std::move(value)); // std::move : lvalue를 rvalue로 변환
		_condVar.notify_one(); // 조건변수를 사용하여 대기중인 스레드를 깨움
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex); // lock_guard: mutex를 lock하고 unlock하는 것을 자동으로 해줌
		if (_stack.empty()) 
			return false; // 스택이 비어있으면 pop 실패로 false 리턴

		value = std::move(_stack.top()); // 스택의 top을 value에 저장
		_stack.pop(); // 스택의 top을 pop
		return true; // 성공적으로 pop을 했으면 true 리턴
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex); // unique_lock : mutex를 lock하고 unlock하는 것을 자동으로 해줌
		_condVar.wait(lock, [this]() { return _stack.empty() == false; }); // 조건변수를 사용하여 스택이 비어있지 않을 때까지 대기
		value = std::move(_stack.top()); // 스택의 top을 value에 저장
		_stack.pop(); // 스택의 top을 pop
	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};