#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize)); // 메모리 풀에서 메모리를 가져온뒤 메모리 헤더를 붙여준다.
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize)); // 메모리 풀에서 메모리를 가져온뒤 메모리 헤더를 붙여준다.
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize)); // 메모리 풀에서 메모리를 가져온뒤 메모리 헤더를 붙여준다.
#endif
		new(memory)Type(forward<Args>(args)...); // placement new : Type의 생성자 호출
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type(); // Type의 소멸자 호출
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		s_pool.Push(MemoryHeader::DetachHeader(obj)); // 메모리 풀에 메모리를 넣음
#endif
	}
	
	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... args)
	{
		shared_ptr<Type> ptr = { Pop(forward<Args>(args)...) , Push };
		return ptr;
	}

private:
	static int32 s_allocSize;
	static MemoryPool s_pool;
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool { s_allocSize };
