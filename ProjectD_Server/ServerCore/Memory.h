#pragma once
#include "Allocator.h"

/*-------------------------------------------
					Memory

   BaseAllocator �� ����Ͽ� �Ҵ� �� ���� �ҋ�
 ��ü�� �����ڿ� �Ҹ��ڸ� ��������� ȣ��� Ŭ����
-------------------------------------------*/
template<typename Type, typename...Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(Xalloc(sizeof(Type)));

	new(memory)Type(forward<Args>(args)...); // placement new : Type�� ������ ȣ��

	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type(); // Type�� �Ҹ��� ȣ��
	Xrelease(obj);
}