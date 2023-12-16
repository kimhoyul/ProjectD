#include "pch.h"
#include "MemoryPool.h"

/*-------------------------------------------
				 MemoryPool

  [32byte][64byte][128byte][256byte][ ... ]
   ������ ũ���� ������ ���� ����ִ� �޸� Ǯ
-------------------------------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
	::InitializeSListHead(&_header); // SList �ʱ�ȭ
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header))) // _header�� ������� �ʴٸ�
		::_aligned_free(memory); // �޸𸮸� �����Ѵ�.
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->allocSize = 0; // �޸𸮸� �ݳ��� allocSIze �� 0 ���� ���� ��޸𸮶� ǥ��

	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr)); // _header�� �޸� �ݳ�

	_useCount.fetch_sub(1); // �Ҵ�� �޸� ������ 1 ���ҽ�Ų��.
	_reserveCount.fetch_add(1); // �ݳ��� �޸� ������ 1 ������Ų��.

}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)); // _header���� �޸𸮸� �����´�.

	if (memory == nullptr) // ����ִٸ�
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT)); // �޸𸮸� �Ҵ��Ѵ�.
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0); // �ִٸ� ��������, ������ ������ allocSize�� 0�� �ƴϸ� ũ����
		_reserveCount.fetch_sub(1); // �ݳ��� �޸� ������ 1 ���ҽ�Ų��.
	}

	_useCount.fetch_add(1); // �Ҵ�� �޸� ������ 1 ������Ų��.

	return memory;
}
