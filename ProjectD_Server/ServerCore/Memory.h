#pragma once
#include "Allocator.h"

class MemoryPool;

/*-------------------------------------------
			    MemoryManager

	        �޸𸮸� �Ѱ� �ϴ� Ŭ����
-------------------------------------------*/
class MemoryManager
{
	enum 
	{
		// �޸� Ǯ�� ����
		// ~1024���� 32����		: 32��
		// ~2048���� 128����		: 8�� 
		// ~4096���� 256����		: 8��
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096 // �޸� Ǯ�� �ִ� ũ�� 
		// MAX_ALLOC_SIZE �̻��� �޸𸮴� �׳� ���Ҵ�
	};

public:
	MemoryManager();
	~MemoryManager();

	void* Allocate(int32 size);
	void Release(void* ptr);

private:
	vector<MemoryPool*> _pools; // �޸� Ǯ�� ����

	// �޸� ũ�⿡ ���� �޸� Ǯ Ž��
	// O(1) �ð� ���⵵�� ã�� ���� ���̺�
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1]; // 0 ~ 4096 ����
};

/*-------------------------------------------
					Memory

   BaseAllocator �� ����Ͽ� �Ҵ� �� ���� �ҋ�
 ��ü�� �����ڿ� �Ҹ��ڸ� ��������� ȣ��� Ŭ����
-------------------------------------------*/
template<typename Type, typename...Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	new(memory)Type(forward<Args>(args)...); // placement new : Type�� ������ ȣ��

	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type(); // Type�� �Ҹ��� ȣ��
	PoolAllocator::Release(obj);
}

template<typename Type>
shared_ptr<Type> MakeShared()
{
	return shared_ptr<Type>{ xnew<Type>(), xdelete<Type> };
}