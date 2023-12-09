#include "pch.h"
#include "Allocator.h"

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
