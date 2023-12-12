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

	// ���� entry(SListEntry)�� �ּҴ� 16��Ʈ ���Ļ����̱⿡ ���� 4��Ʈ�� 0��
	// HeaderX64.next �� 60 ��Ʈ�� ������� ����
	// entry�� ���� 4��Ʈ�� ��Ʈ����Ʈ ������ ���� HeaderX64.next�� 60��Ʈ�� �־���
	desired.HeaderX64.next = ((uint64)entry >> 4);

	while (true)
	{
		expected = *header;

		// expected.HeaderX64.next �� ���� ��Ʈ����Ʈ ������ ���� 4��Ʈ�� �о� �ּҷ� ����
		entry->next = (SListEntry*)(((int64)expected.HeaderX64.next) << 4);
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		// �� ���̿� Ÿ �����忡 ���� �����Ͱ� ����� �� ���� 
	    // ���� CAS�� �����ϸ� while������ ���ư��� �ٽ� ó������ header�� �ּҸ� �����ͼ� �õ�
		// CAS�� �����ϸ� while���� �������ͼ� �Լ��� ����
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

		// Use-After-Free �� �߻� �� �� ����
		desired.HeaderX64.next = ((int64)entry->next) >> 4;
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.aliment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}