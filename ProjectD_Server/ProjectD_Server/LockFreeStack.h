#pragma once

/*-------------------------------------------
				   3�� �õ�

  ABA ������ �ذ��ϱ� ���� ī���͸� �дٴ°� �ٽ�
  16����Ʈ ������ ����Ͽ� ī���͸� �Ѽ� �ְ�	 ��
-------------------------------------------*/
// �޸� �ּ� 16����Ʈ ����
// ���� 4��Ʈ�� 0���� ä����
DECLSPEC_ALIGN(16) 
struct SListEntry
{
	SListEntry* next;
};

// �޸� �ּ� 16����Ʈ ����
// ���� 4��Ʈ�� 0���� ä����
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