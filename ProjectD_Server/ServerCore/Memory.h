#pragma once
#include "Allocator.h"

class MemoryPool;

/*-------------------------------------------
			    MemoryManager

	        메모리를 총괄 하는 클래스
-------------------------------------------*/
class MemoryManager
{
	enum 
	{
		// 메모리 풀의 개수
		// ~1024까지 32단위		: 32개
		// ~2048까지 128단위		: 8개 
		// ~4096까지 256단위		: 8개
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096 // 메모리 풀의 최대 크기 
		// MAX_ALLOC_SIZE 이상의 메모리는 그냥 힙할당
	};

public:
	MemoryManager();
	~MemoryManager();

	void* Allocate(int32 size);
	void Release(void* ptr);

private:
	vector<MemoryPool*> _pools; // 메모리 풀들 모음

	// 메모리 크기에 따라 메모리 풀 탐색
	// O(1) 시간 복잡도로 찾기 위한 테이블
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1]; // 0 ~ 4096 까지
};

/*-------------------------------------------
					Memory

   BaseAllocator 를 사용하여 할당 및 해제 할떄
 객체의 생성자와 소멸자를 명시적으로 호출는 클래스
-------------------------------------------*/
template<typename Type, typename...Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	new(memory)Type(forward<Args>(args)...); // placement new : Type의 생성자 호출

	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type(); // Type의 소멸자 호출
	PoolAllocator::Release(obj);
}

template<typename Type>
shared_ptr<Type> MakeShared()
{
	return shared_ptr<Type>{ xnew<Type>(), xdelete<Type> };
}