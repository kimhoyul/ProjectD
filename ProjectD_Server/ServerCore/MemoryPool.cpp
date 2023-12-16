#include "pch.h"
#include "MemoryPool.h"

/*-------------------------------------------
				 MemoryPool

  [32byte][64byte][128byte][256byte][ ... ]
   동일한 크기의 데이터 끼리 모아주는 메모리 풀
-------------------------------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
	::InitializeSListHead(&_header); // SList 초기화
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header))) // _header가 비어있지 않다면
		::_aligned_free(memory); // 메모리를 해제한다.
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->allocSize = 0; // 메모리를 반납시 allocSIze 를 0 으로 만들어서 빈메모리라 표현

	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr)); // _header에 메모리 반납

	_useCount.fetch_sub(1); // 할당된 메모리 개수를 1 감소시킨다.
	_reserveCount.fetch_add(1); // 반납된 메모리 개수를 1 증가시킨다.

}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)); // _header에서 메모리를 가져온다.

	if (memory == nullptr) // 비어있다면
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT)); // 메모리를 할당한다.
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0); // 있다면 가져오고, 가져온 데이터 allocSize가 0이 아니면 크래시
		_reserveCount.fetch_sub(1); // 반납된 메모리 개수를 1 감소시킨다.
	}

	_useCount.fetch_add(1); // 할당된 메모리 개수를 1 증가시킨다.

	return memory;
}
