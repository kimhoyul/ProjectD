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

/*-------------------------------------------
			    StompAllocator

  메모리 오염 버그를 찾기 쉽게 만든 할당자 클래스
              #디버그 모드에서 사용
-------------------------------------------*/
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};