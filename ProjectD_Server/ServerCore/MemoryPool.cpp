#include "pch.h"
#include "MemoryPool.h"

/*-------------------------------------------
				 MemoryPool

  [32byte][64byte][128byte][256byte][ ... ]
   동일한 크기의 데이터 끼리 모아주는 메모리 풀
-------------------------------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
}

MemoryPool::~MemoryPool()
{
	while (_queue.empty() == false) // 큐가 비어있지 않다면
	{
		MemoryHeader* header = _queue.front(); // 큐의 맨 앞에 있는 데이터를 가져온다.
		_queue.pop(); // 큐의 맨 앞에 있는 데이터를 삭제한다.

		::free(header); // 메모리를 해제한다.
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	WRITE_LOCK;
	ptr->allocSize = 0; // 메모리를 반납시 allocSIze 를 0 으로 만들어서 빈메모리라 표현

	_queue.push(ptr); // 큐에 메모리 반납

	_allocCount.fetch_sub(1); // 할당된 메모리 개수를 1 감소시킨다.

}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* header = nullptr;

	{
		WRITE_LOCK;

		if (_queue.empty() == false) // 큐가 비어있지 않다면
		{
			header = _queue.front(); // 큐의 맨 앞에 있는 데이터를 가져온다.
			_queue.pop(); // 큐의 맨 앞에 있는 데이터를 삭제한다.
		}
	}

	if (header == nullptr) // 큐가 비어있다면
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize)); // 메모리를 할당한다.
	}
	else
	{
		ASSERT_CRASH(header->allocSize == 0); // 가져온 데이터 allocSize가 0이 아니면 크래시
	}

	_allocCount.fetch_add(1); // 할당된 메모리 개수를 1 증가시킨다.

	return header;
}
