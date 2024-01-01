#pragma once

// 서버에서 보낸 패킷을 구분 하기 위한 enum
// S_ 로 시작하는 패킷은 서버에서 보낸 패킷
enum
{
	S_TEST = 1,
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

	static void Handle_S_TEST(BYTE* buffer, int32 len);
};
