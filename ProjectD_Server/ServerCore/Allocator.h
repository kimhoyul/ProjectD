#pragma once

/*-------------------------------------------
				BaseAllocator

 new 와 delete를 오버로딩한 사용할 할당자 클래스
-------------------------------------------*/
class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

