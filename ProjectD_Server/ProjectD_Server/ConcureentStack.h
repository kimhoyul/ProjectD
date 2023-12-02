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

//template<typename T>
//class LockFreeStack // 간지나지만 효율 개똥망인 스택 : 어렵지만 그렇다고 락 효율이 좋은 것도 아님
//{
//	struct Node
//	{
//		Node(const T& value) : data(make_shared<T>(value)), next(nullptr)
//		{
//
//		}
//
//		shared_ptr<T> data;
//		shared_ptr<Node> next;
//	};
//
//public:
//	void Push(const T& value)
//	{
//		shared_ptr<Node> node = make_shared<Node>(value);
//		node->next = std::atomic_load(&_head);
//
//		while (std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
//			;
//	}
//
//	shared_ptr<T> TryPop()
//	{
//		shared_ptr<Node> oldHead = std::atomic_load(&_head);
//
//		while (oldHead && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
//			;
//
//		if (oldHead == nullptr)
//			return shared_ptr<T>();
//
//		return oldHead->data;
//	}
//
//private:
//	shared_ptr<Node> _head;
//};

template<typename T>
class LockFreeStack // 간지나지만 효율 개똥망인 스택 : 어렵지만 그렇다고 락 효율이 좋은 것도 아님
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount = 0;
		Node* ptr = nullptr;
	};

	struct Node
	{
		Node(const T& value) : data(make_shared<T>(value))
		{

		}

		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;
	};

public:
	void Push(const T& value)
	{
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;
		
		// _head에 넣어주기
		node.ptr->next = _head;
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
			;
	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head;
		while (true)
		{
			// 참조권 획득 (externalCount를 현 시점 기준 +1 한 애가 이김)
			IncreaseHeadCount(oldHead);
			// 최소한 externalCount가 2이어야 함 고로 삭제X
			Node* ptr = oldHead.ptr;

			if (ptr == nullptr)
				return shared_ptr<T>();

			// 소유권 획득 (ptr->next 로 head를 바꿔치기 한 애가 이김)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				// 나 말고 또 누가 있는가?
				const int32 countIncrease = oldHead.externalCount - 2;
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr; 

				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)
			{
				// 참조권은 얻었으나, 소유권은 실패 -> 뒷수습은 내가 한다.
				delete ptr;
			}
		}

		while (oldHead.ptr && _head.compare_exchange_weak(oldHead, oldHead.ptr->next) == false)
			;

		if (oldHead.ptr == nullptr)
			return shared_ptr<T>();

		return oldHead.ptr->data;
	}

private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter)
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;

			if (_head.compare_exchange_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head;
};