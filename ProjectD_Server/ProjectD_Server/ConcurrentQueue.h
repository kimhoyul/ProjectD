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

//template<typename T>
//class LockFreeQueue
//{
//	struct Node
//	{
//		shared_ptr<T> data;
//		Node* next = nullptr;
//	};
//
//public:
//	LockFreeQueue() : _head(new Node), _tail(_head) 
//	{
//
//	}
//
//	LockFreeQueue(const LockFreeQueue&) = delete;
//	LockFreeQueue& operator=(const LockFreeQueue&) = delete;
//
//	void Push(const T& value)
//	{
//		shared_ptr<T> newData = make_shared<T>(value);
//
//		Node* dummy = new Node; // dummy node 생성
//
//		Node* oldTail = _tail;
//		oldTail->data.swap(newData); // oldTail의 data와 newData의 data를 swap
//		oldTail->next = dummy; // oldTail의 next를 dummy로 설정
//		
//		_tail = dummy;
//	}
//
//	shared_ptr<T> TryePop()
//	{
//		Node* oldHead = PopHead(); // PopHead를 호출하여 oldHead에 저장
//		if (oldHead == nullptr) // oldHead가 nullptr이면
//			return shared_ptr<T>(); // 빈 값 반환
//
//		shared_ptr<T> res(oldHead->data); // oldHead의 data를 res에 저장
//		delete oldHead; // oldHead 삭제
//
//		return res; // res 반환
//	}
//
//private:
//	Node* PopHead()
//	{
//		Node* oldHead = _head; // oldHead에 _head를 저장
//		if (oldHead == _tail) // _head와 _tail이 같으면 dummy를 가르키고 있으므로 널 반환
//			return nullptr;
//
//		_head = oldHead->next; // _head를 oldHead의 next로 설정
//		return oldHead; // oldHead 반환
//
//	}
//
//private:
//	// [data][data][data][data]
//	//   ^					^
//	// [head]			 [tail]
//	Node* _head;
//	Node* _tail;
//};

template<typename T>
class LockFreeQueue
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount = 0; // 참조권
		Node* ptr = nullptr;
	};

	struct NodeCounter
	{
		uint32 internalCount : 30; // 참조권 반환 관련
		uint32 externalCountRemaining : 2; // Push, Pop 다중 참조권 관련
	};

	struct Node
	{
		Node()
		{
			NodeCounter newCounter;
			newCounter.internalCount = 0;
			newCounter.externalCountRemaining = 2;
			count.store(newCounter);

			next.ptr = nullptr;
			next.externalCount = 0;
		}

		void ReleaseRef()
		{
			NodeCounter oldCounter = count.load();

			while (true)
			{
				NodeCounter newCounter = oldCounter;
				newCounter.internalCount--;

				if (count.compare_exchange_strong(oldCounter, newCounter))
				{
					if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
						delete this;

					break;
				}
			}
		}

		atomic<T*> data;
		atomic<NodeCounter> count;
		CountedNodePtr next;
	};

public:
	LockFreeQueue()
	{
		CountedNodePtr node;
		node.ptr = new Node;
		node.externalCount = 1;

		_head.store(node);
		_tail.store(node);
	}

	LockFreeQueue(const LockFreeQueue&) = delete;
	LockFreeQueue& operator=(const LockFreeQueue&) = delete;

	void Push(const T& value)
	{
		unique_ptr<T> newData = make_unique<T>(value);

		CountedNodePtr dummy;
		dummy.ptr = new Node;
		dummy.externalCount = 1;

		CountedNodePtr oldTail = _tail.load(); // ptr = nullptr

		while (true)
		{
			// 참조권 획득
			IncreaseExternalCount(_tail, oldTail);

			// 소유권 획득
			T* oldData = nullptr; 
			if (oldTail.ptr->data.compare_exchange_strong(oldData, newData.get())) // oldData가 nullptr이면 newData.get()으로 설정
			{
				oldTail.ptr->next = dummy;
				oldTail = _tail.exchange(dummy); // dummy를 _tail에 저장
				
				FreeExternalCounter(oldTail);

				newData.release();
				break;
			}

			// 소유권 경쟁 패배
			oldTail.ptr->ReleaseRef();
		}
	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head.load();

		while (true)
		{
			// 참조권 획득
			IncreaseExternalCount(_head, oldHead);
			
			Node* ptr = oldHead.ptr;
			if (ptr == _tail.load().ptr)
			{
				ptr->ReleaseRef();
				return shared_ptr<T>();
			}

			// 소유권 획득
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				T* res = ptr->data.exchange(nullptr);
				FreeExternalCounter(oldHead);
				return shared_ptr<T>(res);
			}

			// 소유권 경쟁 패배
			ptr->ReleaseRef();
		}
	}

private:
	static void IncreaseExternalCount(atomic<CountedNodePtr>& counter, CountedNodePtr& oldCounter)
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;

			if (counter.compare_exchange_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

	static void FreeExternalCounter(CountedNodePtr& oldNodePtr)
	{
		Node* ptr = oldNodePtr.ptr;
		const int32 countIncrease = oldNodePtr.externalCount - 2;

		NodeCounter oldCounter = ptr->count.load();
		
		while (true)
		{
			NodeCounter newCounter = oldCounter;
			newCounter.externalCountRemaining--;
			newCounter.internalCount += countIncrease;

			if (ptr->count.compare_exchange_strong(oldCounter, newCounter))
			{
				if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
					delete ptr;

				break;
			}
		}
	}

private:
	// [data][data][data][data]
	//   ^					^
	// [head]			 [tail]
	atomic<CountedNodePtr> _head;
	atomic<CountedNodePtr> _tail;
};