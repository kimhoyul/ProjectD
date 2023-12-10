#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*-------------------------------------------
				BaseAllocator

		  기본으로 사용할 할당자 클래스
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

  메모리 오염 버그를 찾기 쉽게 만든 할당자 클래스
-------------------------------------------*/
void* StompAllocator::Alloc(int32 size)
{
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE; 
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddres = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);  // 메모리를 할당한후 시작 주소를 반환
	return static_cast<void*>(static_cast<int8*>(baseAddres) + dataOffset); // 시작 주소 + 데이터 오프셋
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

/*-------------------------------------------
				PoolAllocator

		메모리 풀에 사용할 할당자 클래스
-------------------------------------------*/
void* PoolAllocator::Alloc(int32 size)
{
	return GMemoryManager->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemoryManager->Release(ptr);
}