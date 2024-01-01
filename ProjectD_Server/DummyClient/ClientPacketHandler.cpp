#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

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

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	Protocol::S_TEST pkt;

	ASSERT_CRASH(pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)));

	cout << pkt.id() << " " << pkt.hp() << " " << pkt.attack() << endl;

	cout << "BUFFSIZE : " << pkt.buffs_size() << endl;
	for (auto& buff : pkt.buffs())
	{
		cout << "BUFFINFO : " << buff.buffid() << " " << buff.remaintime() << endl;

		cout << "VICTIMSIZE : " << buff.victims_size() << endl;
		for (auto& victim : buff.victims())
		{
			cout << "VICTIM : " << victim << ", ";
		}

		cout << endl;
	}

}
