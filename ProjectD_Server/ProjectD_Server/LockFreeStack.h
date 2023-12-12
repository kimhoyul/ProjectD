#pragma once

/*-------------------------------------------
				   3차 시도

  ABA 문제를 해결하기 위해 카운터를 둔다는게 핵심
  16바이트 정렬을 사용하여 카운터를 둘수 있게	 함
-------------------------------------------*/
// 메모리 주소 16바이트 정렬
// 하위 4비트는 0으로 채워짐
DECLSPEC_ALIGN(16) 
struct SListEntry
{
	SListEntry* next;
};

// 메모리 주소 16바이트 정렬
// 하위 4비트는 0으로 채워짐
DECLSPEC_ALIGN(16) 
struct SListHeader
{
	SListHeader()
	{
		aliment = 0;
		region = 0;
	}

	union 
	{
		struct
		{
			uint64 aliment;
			uint64 region;
		} DUMMYSTRUCTNAME;
		struct 
		{
			uint64 depth : 16;
			uint64 sequence : 48;
			uint64 reserved : 4;
			uint64 next : 60;
		} HeaderX64;
	};
};

void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);