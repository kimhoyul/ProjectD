#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*-------------------------------------------
				BaseAllocator

		  �⺻���� ����� �Ҵ��� Ŭ����
-------------------------------------------*/
void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}

/*-------------------------------------------
			   StompAllocator

  �޸� ���� ���׸� ã�� ���� ���� �Ҵ��� Ŭ����
-------------------------------------------*/
void* StompAllocator::Alloc(int32 size)
{
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE; 
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddres = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);  // �޸𸮸� �Ҵ����� ���� �ּҸ� ��ȯ
	return static_cast<void*>(static_cast<int8*>(baseAddres) + dataOffset); // ���� �ּ� + ������ ������
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

/*-------------------------------------------
				PoolAllocator

		�޸� Ǯ�� ����� �Ҵ��� Ŭ����
-------------------------------------------*/
void* PoolAllocator::Alloc(int32 size)
{
	return GMemoryManager->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemoryManager->Release(ptr);
}