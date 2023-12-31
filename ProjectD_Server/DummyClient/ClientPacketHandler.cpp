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
		// [고정 데이터(PKT_S_TEST)][가변데이터(BuffsListItem)(BuffsListItem)(BuffsListItem)]
		// ^                       ^                                                         
		// |--------- size --------|
		if (packetSize < size)
			return false; 

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

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;

	BuffsList GetBuffsList()
	{
		// 바이트 포인터로 만들어준후
		BYTE* data = reinterpret_cast<BYTE*>(this);
		// 버퍼 오프셋을 더해준다. 
		// 바이트 포인터는 1바이트씩 이동한다.
		// 버퍼 오프셋만큼 더해주면 가변 데이터의 시작 위치가 된다.
		data += buffsOffeset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
	}
	// 가변 데이터
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
