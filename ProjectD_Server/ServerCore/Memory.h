#pragma once
#include "Allocator.h"

/*-------------------------------------------
					Memory

   BaseAllocator 를 사용하여 할당 및 해제 할떄
 객체의 생성자와 소멸자를 명시적으로 호출는 클래스
-------------------------------------------*/
template<typename Type, typename...Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(Xalloc(sizeof(Type)));

	new(memory)Type(forward<Args>(args)...); // placement new : Type의 생성자 호출

	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type(); // Type의 소멸자 호출
	Xrelease(obj);
}