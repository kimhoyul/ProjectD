#include "pch.h"
#include <iostream>

#include <WinSock2.h> // for socket
#include <MSWSock.h> // for AcceptEx
#include <ws2tcpip.h> // for inet_ntop
#pragma comment(lib, "ws2_32.lib") // Winsock Library

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " : " << errCode << endl;
}

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;
	
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("socket()");
		return 0;
	}

	u_long on = 1;
	// set non-blocking
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		HandleError("ioctlsocket()");
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			
			// 이미 연결된 상태라면 break;
			if (::WSAGetLastError() == WSAEISCONN)
				break;

			HandleError("connect()");
			break;
		}
	}

	cout << "Connected!" << endl;

	char sendBuffer[100] = "Hello World!";

	while (true)
	{
		if (::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			// 원래 블록했어야 했지만 논블록으로 바꿔서 SOCKET_ERROR 빠질 수 있음
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			HandleError("send()");
			break;
		}

		cout << "Send Data Len : " << sizeof(sendBuffer) << " | Send Data : " << sendBuffer << endl;

		while (true)
		{
			char recvBuffer[1000];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				// 원래 블록했어야 했지만 논블록으로 바꿔서 SOCKET_ERROR 빠질 수 있음
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;

				HandleError("recv()");
				break;
			}
			else if (recvLen == 0)
			{
				cout << "Server Disconnected" << endl;
				break;
			}

			cout << "Recv Data Len : " << recvLen << " | Recv Data : " << recvBuffer << endl;
			break;
		}

		this_thread::sleep_for(1s);
	}

	

	::closesocket(clientSocket);
	::WSACleanup();
}
