#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"

enum
{
	S_TEST = 1
};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

	static SendBufferRef MakeSendBuffer(Protocol::S_TEST& pkt);
};

template<typename T>
SendBufferRef _MakeSendBuffer(T& pkt, uint16 pktId)
{
	// Protobuf 를 이용해 만든 데이터의 크기
	const uint16 dataSIze = static_cast<uint16>(pkt.ByteSizeLong());
	// 패킷 헤더 + 데이터 크기 = 최종 데이터 크기
	const uint16 packetSize = dataSIze + sizeof(PacketHeader);
	
	// 패킷 헤더 + 데이터 크기 만큼 버퍼를 할당
	SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);

	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	header->size = packetSize;
	header->id = pktId;

	ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSIze));

	sendBuffer->Close(packetSize);
	return sendBuffer;
}