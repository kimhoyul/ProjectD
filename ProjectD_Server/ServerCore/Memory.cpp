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

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// MAX_ALLOC_SIZE ũ�⸦ ��� �޸𸮴� �׳� malloc���� �Ҵ��Ѵ�.
		header = reinterpret_cast<MemoryHeader*>(malloc(allocSize));
	}
	else
	{
		// Ǯ���� �޸𸮸� �Ҵ��Ѵ�.
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
		// MAX_ALLOC_SIZE ũ�⸦ ��� �޸𸮴� �׳� free�� �����Ѵ�.
		::free(header);
	}
	else
	{
		// Ǯ�� �޸𸮸� �����Ѵ�.
		_poolTable[allocSize]->Push(header);
	}
}
