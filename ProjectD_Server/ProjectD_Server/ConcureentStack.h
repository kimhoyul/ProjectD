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

//template<typename T>
//class LockFreeStack // ���������� ȿ�� ���˸��� ���� : ������� �׷��ٰ� �� ȿ���� ���� �͵� �ƴ�
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
class LockFreeStack // ���������� ȿ�� ���˸��� ���� : ������� �׷��ٰ� �� ȿ���� ���� �͵� �ƴ�
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
		
		// _head�� �־��ֱ�
		node.ptr->next = _head;
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
			;
	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head;
		while (true)
		{
			// ������ ȹ�� (externalCount�� �� ���� ���� +1 �� �ְ� �̱�)
			IncreaseHeadCount(oldHead);
			// �ּ��� externalCount�� 2�̾�� �� ��� ����X
			Node* ptr = oldHead.ptr;

			if (ptr == nullptr)
				return shared_ptr<T>();

			// ������ ȹ�� (ptr->next �� head�� �ٲ�ġ�� �� �ְ� �̱�)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				// �� ���� �� ���� �ִ°�?
				const int32 countIncrease = oldHead.externalCount - 2;
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr; 

				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)
			{
				// �������� �������, �������� ���� -> �޼����� ���� �Ѵ�.
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