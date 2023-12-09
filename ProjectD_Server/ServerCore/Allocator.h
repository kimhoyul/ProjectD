#pragma once

/*-------------------------------------------
				BaseAllocator

 new �� delete�� �����ε��� ����� �Ҵ��� Ŭ����
-------------------------------------------*/
class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

/*-------------------------------------------
			    StompAllocator

  �޸� ���� ���׸� ã�� ���� ���� �Ҵ��� Ŭ����
              #����� ��忡�� ���
-------------------------------------------*/
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};