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
		// [���� ������(PKT_S_TEST)][����������(BuffsListItem)(BuffsListItem)(BuffsListItem)]
		// ^                       ^                                                         
		// |--------- size --------|
		if (packetSize < size)
			return false; 

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

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;

	BuffsList GetBuffsList()
	{
		// ����Ʈ �����ͷ� ���������
		BYTE* data = reinterpret_cast<BYTE*>(this);
		// ���� �������� �����ش�. 
		// ����Ʈ �����ʹ� 1����Ʈ�� �̵��Ѵ�.
		// ���� �����¸�ŭ �����ָ� ���� �������� ���� ��ġ�� �ȴ�.
		data += buffsOffeset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
	}
	// ���� ������
	//vector<BuffData> buffs;
	//wstring name;
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	if (pkt->Validate() == true)
		return;

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

	cout << "Buff Count : " << buffs.Count() << endl;
	for (int32 i = 0; i < buffs.Count(); ++i)
	{
		cout << "BuffInfo : " << buffs[i].bufferId << " Remain Time : " << buffs[i].remainTime << endl;
	}

	for (auto it = buffs.begin(); it != buffs.end(); ++it)
	{
		cout << "BuffInfo : " << it->bufferId << " Remain Time : " << it->remainTime << endl;
	}

	for (auto& buff : buffs)
	{
		cout << "BuffInfo : " << buff.bufferId << " Remain Time : " << buff.remainTime << endl;
	}
}
