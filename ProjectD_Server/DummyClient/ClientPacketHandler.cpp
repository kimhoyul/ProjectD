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
// 패킷 설계 TEMP
// [고정 데이터(PKT_S_TEST)][가변데이터(BuffsListItem)(BuffsListItem)(BuffsListItem)]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 bufferId;
		float remainTime;
	};

	uint16 packetSize; // 공용 헤더
	uint16 packetId; // 공용 헤더

	// 고정 데이터
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
		// [고정 데이터(PKT_S_TEST)][가변데이터(BuffsListItem)(BuffsListItem)(BuffsListItem)]
		// ^                                                                                ^
		// |------------------------------------ size --------------------------------------|
		if (size != packetSize)
			return false;

		//[고정 데이터(PKT_S_TEST)][가변데이터(BuffsListItem)(BuffsListItem)(BuffsListItem)]
		//                         ^                                                       ^
		//                         buffsOffeset				   buffeOffeset + sizeof(BuffsListItem) * buffsCount
		if (buffsOffeset + sizeof(BuffsListItem) * buffsCount)
			return false;

		return true;
	}
	// 가변 데이터
	//vector<BuffData> buffs;
	//wstring name;
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	// 패킷데이터 보다 작은 경우는 패킷이 아님
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
