#include "pch.h"
#include "MemoryPool.h"

/*-------------------------------------------
				 MemoryPool

  [32byte][64byte][128byte][256byte][ ... ]
   ������ ũ���� ������ ���� ����ִ� �޸� Ǯ
-------------------------------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
}

MemoryPool::~MemoryPool()
{
	while (_queue.empty() == false) // ť�� ������� �ʴٸ�
	{
		MemoryHeader* header = _queue.front(); // ť�� �� �տ� �ִ� �����͸� �����´�.
		_queue.pop(); // ť�� �� �տ� �ִ� �����͸� �����Ѵ�.

		::free(header); // �޸𸮸� �����Ѵ�.
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	WRITE_LOCK;
	ptr->allocSize = 0; // �޸𸮸� �ݳ��� allocSIze �� 0 ���� ���� ��޸𸮶� ǥ��

	_queue.push(ptr); // ť�� �޸� �ݳ�

	_allocCount.fetch_sub(1); // �Ҵ�� �޸� ������ 1 ���ҽ�Ų��.

}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* header = nullptr;

	{
		WRITE_LOCK;

		if (_queue.empty() == false) // ť�� ������� �ʴٸ�
		{
			header = _queue.front(); // ť�� �� �տ� �ִ� �����͸� �����´�.
			_queue.pop(); // ť�� �� �տ� �ִ� �����͸� �����Ѵ�.
		}
	}

	if (header == nullptr) // ť�� ����ִٸ�
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize)); // �޸𸮸� �Ҵ��Ѵ�.
	}
	else
	{
		ASSERT_CRASH(header->allocSize == 0); // ������ ������ allocSize�� 0�� �ƴϸ� ũ����
	}

	_allocCount.fetch_add(1); // �Ҵ�� �޸� ������ 1 ������Ų��.

	return header;
}
