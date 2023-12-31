#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST :
		Handle_S_TEST(buffer, len);
		break;


	default:
		break;
	}
}

#pragma pack(1)
// ��Ŷ ���� TEMP
// [���� ������(PKT_S_TEST)][����������(BuffsListItem)(BuffsListItem)(BuffsListItem)]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 bufferId;
		float remainTime;
	};

	uint16 packetSize; // ���� ���
	uint16 packetId; // ���� ���

	// ���� ������
	uint64 id;
	uint32 hp;
	uint16 attack;

	uint16 buffsOffeset;
	uint16 buffsCount;

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		size += sizeof(BuffsListItem) * buffsCount;
		// [���� ������(PKT_S_TEST)][����������(BuffsListItem)(BuffsListItem)(BuffsListItem)]
		// ^                                                                                ^
		// |------------------------------------ size --------------------------------------|
		if (size != packetSize)
			return false;

		//[���� ������(PKT_S_TEST)][����������(BuffsListItem)(BuffsListItem)(BuffsListItem)]
		//                         ^                                                       ^
		//                         buffsOffeset				   buffeOffeset + sizeof(BuffsListItem) * buffsCount
		if (buffsOffeset + sizeof(BuffsListItem) * buffsCount)
			return false;

		return true;
	}
	// ���� ������
	//vector<BuffData> buffs;
	//wstring name;
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	// ��Ŷ������ ���� ���� ���� ��Ŷ�� �ƴ�
	if (len < sizeof(PKT_S_TEST))
		return;

	PacketHeader header;
	br >> header;

	PKT_S_TEST pkt;
	br >> pkt;

	if (pkt.Validate() == true)
		return;

	uint64 id;
	uint32 hp;
	uint16 attack;
	br >> id >> hp >> attack;

	cout << "ID : " << id << " HP : " << hp << " ATT : " << attack << endl;

	vector<PKT_S_TEST::BuffsListItem> buffs;
	
	buffs.resize(pkt.buffsCount);
	for (int32 i = 0; i < pkt.buffsCount; ++i)
	{
		br >> buffs[i];
	}

	cout << "Buff Count : " << pkt.buffsCount << endl;
	for (int32 i = 0; i < pkt.buffsCount; ++i)
	{
		cout << "BuffInfo : " << buffs[i].bufferId << " Remain Time : " << buffs[i].remainTime << endl;
	}
}
