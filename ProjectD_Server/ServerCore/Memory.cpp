#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

/*-------------------------------------------
				MemoryManager

			메모리를 총괄 하는 클래스
-------------------------------------------*/

MemoryManager::MemoryManager()
{
	int32 size = 0;
	int32 tableIndex = 0;

	// 32 바이트 단위로 ~1024 바이트까지 풀을 만든다. : 32개
	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		// 32 바이트 단위로 각 풀을 참조하도록 테이블을 설정한다.
		while (tableIndex <= size) 
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 128 바이트 단위로 ~2048 바이트까지 풀을 만든다. : 8개
	for (; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		// 128 바이트 단위로 각 풀을 참조하도록 테이블을 설정한다.

		while (tableIndex <= size) 
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 256 바이트 단위로 4096 바이트까지 풀을 만든다. : 8개
	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		// 256 바이트 단위로 각 풀을 참조하도록 테이블을 설정한다.

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

MemoryManager::~MemoryManager()
{
	for (MemoryPool* pool : _pools)
		delete pool;

	_pools.clear();
}

void* MemoryManager::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// MAX_ALLOC_SIZE 크기를 벗어난 메모리는 그냥 malloc으로 할당한다.
		header = reinterpret_cast<MemoryHeader*>(malloc(allocSize));
	}
	else
	{
		// 풀에서 메모리를 할당한다.
		header = _poolTable[allocSize]->Pop();
	}

	return MemoryHeader::AttachHeader(header, allocSize);
}

void MemoryManager::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// MAX_ALLOC_SIZE 크기를 벗어난 메모리는 그냥 free로 해제한다.
		::free(header);
	}
	else
	{
		// 풀에 메모리를 해제한다.
		_poolTable[allocSize]->Push(header);
	}
}
