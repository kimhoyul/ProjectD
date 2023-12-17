#include "pch.h"
#include <iostream>

#include <WinSock2.h> // for socket
#include <MSWSock.h> // for AcceptEx
#include <ws2tcpip.h> // for inet_ntop
#pragma comment(lib, "ws2_32.lib") // Winsock Library

int main()
{
	// ���� �ʱ�ȭ(ws2_32.dll�� �ʱ�ȭ)
	// ���� ������ wsaData�� �����
	WSAData wsaData;
	
	// �����ϸ� 0�� ��ȯ
	// WSAStartup�� ���������� ȣ���� �Ŀ��� �߰� Windows ���� �Լ��� ������ �� ����
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 0;
		cout << "WSAStartup failed" << endl;
	}
	
	// ���� ����
	// �����ϸ� ���� �ڵ��� ��ȯ
	SOCKET clientSocket = ::socket
		(
			AF_INET,		// �ּ� ü�� (Address Family : IPv4)
			SOCK_STREAM,	// ��� ��� (Type : SOCK_STREAM - TCP)
			0				// �������� (protocal : 0 - Auto)
		);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "socket failed : " << errCode << endl;
		return 0;
	}

	SOCKADDR_IN serverAddr; // ���� �ּ� IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr)); // 0���� �ʱ�ȭ
	serverAddr.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // IP �ּ� (�Ž�)
	// little endian / big endian �̽� ������ htons�� ���
	serverAddr.sin_port = ::htons(7777); // Port ��ȣ (Host To Network Short)

	// ������ ����
	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "connect failed : " << errCode << endl;
		return 0;
	}

	// --------------------------------
	// ���� ���� !
	cout << "connect success" << endl;

	while (true)
	{
		//TODO: �����κ��� �����͸� �޴´�.

		this_thread::sleep_for(1s);
	}
	// --------------------------------

	// ���� �ݱ�
	::closesocket(clientSocket);

	// ���� ����
	::WSACleanup();
}
