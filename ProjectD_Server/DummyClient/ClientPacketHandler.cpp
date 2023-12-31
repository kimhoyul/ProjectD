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

		//[고정 데이터(PKT_S_TEST)][가변데이터(BuffsListItem)(BuffsListItem)(BuffsListItem)]
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

		// [고정 데이터(PKT_S_TEST)][가변데이터(BuffsListItem)(BuffsListItem)(BuffsListItem)]
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
		// 바이트 포인터로 만들어준후
		BYTE* data = reinterpret_cast<BYTE*>(this);
		// 버퍼 오프셋을 더해준다. 
		// 바이트 포인터는 1바이트씩 이동한다.
		// 버퍼 오프셋만큼 더해주면 가변 데이터의 시작 위치가 된다.
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
