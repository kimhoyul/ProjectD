#pragma once

// �������� ���� ��Ŷ�� ���� �ϱ� ���� enum
// S_ �� �����ϴ� ��Ŷ�� �������� ���� ��Ŷ
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
