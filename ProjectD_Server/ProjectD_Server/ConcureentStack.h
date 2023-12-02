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

template<typename T>
class LockFreeStack // 간지나지만 효율 개똥망인 스택 : 어렵지만 그렇다고 락 효율이 좋은 것도 아님
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
	// 1) 새 노드를 만든다.
	// 2) 새 노드의 next = _head
	// 3) _head = 새 노드
	void Push(const T& value)
	{
		Node* node = new Node(value); // 1) 새 노드를 만든다.
		node->next = _head.load(); // 2) 새 노드의 next = _head


		// 윗줄의 코드에서 node->next = _head.load() 변경 하였으나 
		// 본 스레드가 [ 3) _head = 새 노드 ] 를 하기전 다른 스레드가 윗줄을 통해 _head를 변경할 수 있으므로
		// CAS(Compare And Swap)을 사용하여 변경
		/* if (_head == node->next)  // 현재 기준으로 _head와 node->next가 같으면
		{
			_head = node; // 3) _head = 새 노드 를 해주고
			return true;
		}
		else // 현재 기준으로 _head와 node->next가 다르면
		{
			node->next = _head; // 2) 새 노드의 next = _head 를 다시 해주고
			return false;
		}*/
		while (_head.compare_exchange_weak(node->next, node) == false); // 3) _head = 새 노드
	}

	// OUT<-[X] [ ][ ][ ][ ][ ][ ]
	//           ^
	//           [_head]
	// 1) oldHead = _head
	// 2) _head = _head->next
	// 3) 기존 _head data 추출 / 반환
	// 4) 기존 _head 삭제
	bool TryPop(T& value)
	{
		++_popcount; // popcount 증가

		Node* oldHead = _head.load(); // 1) oldHead = _head
		
		// 윗줄의 코드에서 oldHead = _head.load() 하였으나 
		// 본 스레드가 [ 2) _head = _head->next ] 를 하기전 다른 스레드가 윗줄을 통해 _head를 변경할 수 있으므로
		// CAS(Compare And Swap)을 사용하여 변경
		/*if (_head == oldHead) // 현재 기준으로 _head와 oldHead가 같으면 
		{
			_head = _head->next; // 2) _head = _head->next
			return true;
		}
		else // 현재 기준으로 _head와 oldHead가 다르면
		{
			oldHead = _head; // 1) oldHead = _head
			return false;
		}*/
		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false); // 2) _head = _head->next
		
		if (oldHead == nullptr) // oldHead가 nullptr이면
		{
			--_popcount; // popcount 감소
			return false; // false 리턴
		}

		value = oldHead->data; // 3) 기존 _head data 추출 / 반환
		TryDelete(oldHead); // 4) 기존 _head 삭제

		return true;
	}

	void TryDelete(Node* oldHead)
	{
		// 나 외에 누가 이 함수에 들어와 있는지?
		if (_popcount == 1) // 나만 들어와 있으면
		{
			// 나만 있는게 확실하니, 삭제 예약된 다른 데이터도 삭제하자
			Node* node = _pendingList.exchange(nullptr); 

			if (--_popcount == 0) // 끼어든 애가 없다면
			{
				// 삭제 진행
				DeleteNodes(node);
			}	
			else if (node)// 누가 끼어들었으니 다시 데이터 반환
			{
				ChainPendingNodeList(node);
			}

			// 내꺼 데이터 삭제
			delete oldHead;
		}
		else 
		{
			// 누가 있네? 그럼 지금 삭제하지 않고 삭제 예약만
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

	// 위 함수를 오버로딩한 헬퍼 함수
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
		//  삭제
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