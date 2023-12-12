#include "pch.h"
#include "LockFreeStack.h"

void InitializeHead(SListHeader* header)
{
	header->aliment = 0;
	header->region = 0;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	SListHeader expected = {};
	SListHeader desired = {};

	// 현재 entry(SListEntry)의 주소는 16비트 정렬상태이기에 하위 4비트가 0임
	// HeaderX64.next 는 60 비트로 만들어져 있음
	// entry의 하위 4비트를 비트쉬프트 연산을 통해 HeaderX64.next의 60비트에 넣어줌
	desired.HeaderX64.next = ((uint64)entry >> 4);

	while (true)
	{
		expected = *header;

		// expected.HeaderX64.next 를 왼쪽 비트쉬프트 연산을 통해 4비트를 밀어 주소로 복원
		entry->next = (SListEntry*)(((int64)expected.HeaderX64.next) << 4);
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		// 이 사이에 타 스레드에 의해 데이터가 변경될 수 있음 
	    // 만약 CAS가 실패하면 while문으로 돌아가서 다시 처음부터 header의 주소를 가져와서 시도
		// CAS가 성공하면 while문을 빠져나와서 함수를 종료
		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.aliment, (int64*)&expected) == 1)
			break;
	}
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListHeader expected = {};
	SListHeader desired = {};
	SListEntry* entry = nullptr;

	while (true)
	{
		expected = *header;

		entry = (SListEntry*)(((int64)expected.HeaderX64.next) << 4);
		if (entry == nullptr)
			break;

		// Use-After-Free 가 발생 할 수 있음
		desired.HeaderX64.next = ((int64)entry->next) >> 4;
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.aliment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}