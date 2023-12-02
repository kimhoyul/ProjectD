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

template<typename T>
class LockFreeStack // ���������� ȿ�� ���˸��� ���� : ������� �׷��ٰ� �� ȿ���� ���� �͵� �ƴ�
{
	struct Node
	{
		Node(const T& value) : data(value), next(nullptr)
		{

		}

		T data;
		Node* next;
	};

public:
	// [value][ ][ ][ ][ ][ ][ ]
	//  ^  
	// [_head]
	// 1) �� ��带 �����.
	// 2) �� ����� next = _head
	// 3) _head = �� ���
	void Push(const T& value)
	{
		Node* node = new Node(value); // 1) �� ��带 �����.
		node->next = _head.load(); // 2) �� ����� next = _head


		// ������ �ڵ忡�� node->next = _head.load() ���� �Ͽ����� 
		// �� �����尡 [ 3) _head = �� ��� ] �� �ϱ��� �ٸ� �����尡 ������ ���� _head�� ������ �� �����Ƿ�
		// CAS(Compare And Swap)�� ����Ͽ� ����
		/* if (_head == node->next)  // ���� �������� _head�� node->next�� ������
		{
			_head = node; // 3) _head = �� ��� �� ���ְ�
			return true;
		}
		else // ���� �������� _head�� node->next�� �ٸ���
		{
			node->next = _head; // 2) �� ����� next = _head �� �ٽ� ���ְ�
			return false;
		}*/
		while (_head.compare_exchange_weak(node->next, node) == false); // 3) _head = �� ���
	}

	// OUT<-[X] [ ][ ][ ][ ][ ][ ]
	//           ^
	//           [_head]
	// 1) oldHead = _head
	// 2) _head = _head->next
	// 3) ���� _head data ���� / ��ȯ
	// 4) ���� _head ����
	bool TryPop(T& value)
	{
		++_popcount; // popcount ����

		Node* oldHead = _head.load(); // 1) oldHead = _head
		
		// ������ �ڵ忡�� oldHead = _head.load() �Ͽ����� 
		// �� �����尡 [ 2) _head = _head->next ] �� �ϱ��� �ٸ� �����尡 ������ ���� _head�� ������ �� �����Ƿ�
		// CAS(Compare And Swap)�� ����Ͽ� ����
		/*if (_head == oldHead) // ���� �������� _head�� oldHead�� ������ 
		{
			_head = _head->next; // 2) _head = _head->next
			return true;
		}
		else // ���� �������� _head�� oldHead�� �ٸ���
		{
			oldHead = _head; // 1) oldHead = _head
			return false;
		}*/
		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false); // 2) _head = _head->next
		
		if (oldHead == nullptr) // oldHead�� nullptr�̸�
		{
			--_popcount; // popcount ����
			return false; // false ����
		}

		value = oldHead->data; // 3) ���� _head data ���� / ��ȯ
		TryDelete(oldHead); // 4) ���� _head ����

		return true;
	}

	void TryDelete(Node* oldHead)
	{
		// �� �ܿ� ���� �� �Լ��� ���� �ִ���?
		if (_popcount == 1) // ���� ���� ������
		{
			// ���� �ִ°� Ȯ���ϴ�, ���� ����� �ٸ� �����͵� ��������
			Node* node = _pendingList.exchange(nullptr); 

			if (--_popcount == 0) // ����� �ְ� ���ٸ�
			{
				// ���� ����
				DeleteNodes(node);
			}	
			else if (node)// ���� ���������� �ٽ� ������ ��ȯ
			{
				ChainPendingNodeList(node);
			}

			// ���� ������ ����
			delete oldHead;
		}
		else 
		{
			// ���� �ֳ�? �׷� ���� �������� �ʰ� ���� ���ุ
			ChainPendingNode(oldHead);
			--_popcount;
		}
	}

	// [first][ ][ ][ ][ ][ ][last] -> [_pendingList][ ][ ][ ][ ]...
	//   ^                         
	// [_pendingList]              
	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{

		}
	}

	// �� �Լ��� �����ε��� ���� �Լ�
	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;
		while (last->next)
			last = last->next;

		ChainPendingNodeList(node, last);
	}

	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node)
	{
		// [node] < [next] < [next] < ...
		//   ^  
		//  ����
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}

private:
	// [ ][ ][ ][ ][ ][ ][ ]
	//  ^  
	// [_head]
	atomic<Node*> _head;

	atomic<uint32> _popcount = 0;
	atomic<Node*> _pendingList;
};