#include "pch.h"
#include "LockFreeStack.h"

void InitializeHead(SListHeader* header)
{
	header->next = nullptr;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next;

	// Desired, Expected, ´ë»ó
	while (::InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next) == 0)
	{
		
	}
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListEntry* expected = header->next;

	// ABA Problem


	while (expected != nullptr && ::InterlockedCompareExchange64((int64*)&header->next, (int64)expected->next, (int64)expected) == 0)
	{
		
	}

	return expected;
}