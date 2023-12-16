#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

/*-------------------------------------------
				MemoryManager

			�޸𸮸� �Ѱ� �ϴ� Ŭ����
-------------------------------------------*/

MemoryManager::MemoryManager()
{
	int32 size = 0;
	int32 tableIndex = 0;

	// 32 ����Ʈ ������ ~1024 ����Ʈ���� Ǯ�� �����. : 32��
	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		// 32 ����Ʈ ������ �� Ǯ�� �����ϵ��� ���̺��� �����Ѵ�.
		while (tableIndex <= size) 
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 128 ����Ʈ ������ ~2048 ����Ʈ���� Ǯ�� �����. : 8��
	for (; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		// 128 ����Ʈ ������ �� Ǯ�� �����ϵ��� ���̺��� �����Ѵ�.

		while (tableIndex <= size) 
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 256 ����Ʈ ������ 4096 ����Ʈ���� Ǯ�� �����. : 8��
	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		// 256 ����Ʈ ������ �� Ǯ�� �����ϵ��� ���̺��� �����Ѵ�.

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
#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// MAX_ALLOC_SIZE ũ�⸦ ��� �޸𸮴� �׳� malloc���� �Ҵ��Ѵ�.
		header = reinterpret_cast<MemoryHeader*>(_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// Ǯ���� �޸𸮸� �Ҵ��Ѵ�.
		header = _poolTable[allocSize]->Pop();
	}
#endif
	

	return MemoryHeader::AttachHeader(header, allocSize);
}

void MemoryManager::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

#ifdef _STOMP
	StompAllocator::Release(header);
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// MAX_ALLOC_SIZE ũ�⸦ ��� �޸𸮴� �׳� free�� �����Ѵ�.
		::_aligned_free(header);
	}
	else
	{
		// Ǯ�� �޸𸮸� �����Ѵ�.
		_poolTable[allocSize]->Push(header);
	}
#endif
	
}
