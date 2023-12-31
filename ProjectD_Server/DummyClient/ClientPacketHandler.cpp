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

		uint16 victimsOffset;
		uint16 victimsCount;

		bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size)
		{
			if (victimsOffset + victimsCount * sizeof(uint64) > packetSize)
				return false;

			size += victimsCount * sizeof(uint64);

			return true;
		}
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

		//[���� ������(PKT_S_TEST)][����������(BuffsListItem)(BuffsListItem)(BuffsListItem)]
		//                         ^                                                       ^
		//                         buffsOffeset				   buffeOffeset + sizeof(BuffsListItem) * buffsCount
		if (buffsOffeset + sizeof(BuffsListItem) * buffsCount > packetSize)
			return false;

		size += sizeof(BuffsListItem) * buffsCount;

		BuffsList buffList = GetBuffsList();
		for (int32 i = 0; i < buffList.Count(); ++i)
		{
			if (buffList[i].Validate((BYTE*)this, packetSize, size) == false)
				return false;
		}

		// [���� ������(PKT_S_TEST)][����������(BuffsListItem)(BuffsListItem)(BuffsListItem)]
		// ^                                                                                ^
		// |------------------------------------ size --------------------------------------|
		if (size != packetSize)
			return false;

		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using BuffsVictimsList = PacketList<uint64>;

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

	BuffsVictimsList GetBuffVictimsList(BuffsListItem* buffsItem)
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsItem->victimsOffset;
		return BuffsVictimsList(reinterpret_cast<uint64*>(data), buffsItem->victimsCount);
	}
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	if (pkt->Validate() == false)
		return;

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

	cout << "Buff Count : " << buffs.Count() << endl;

	for (auto& buff : buffs)
	{
		cout << "BuffInfo : " << buff.bufferId << " Remain Time : " << buff.remainTime << endl;

		PKT_S_TEST::BuffsVictimsList buffsVictimsList = pkt->GetBuffVictimsList(&buff);

		cout << "Victims Count : " << buffsVictimsList.Count() << endl;

		for (auto& victim : buffsVictimsList)
		{
			cout << "Victim : " << victim << endl;
		}
	}
}
