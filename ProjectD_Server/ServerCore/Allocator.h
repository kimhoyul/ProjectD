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

