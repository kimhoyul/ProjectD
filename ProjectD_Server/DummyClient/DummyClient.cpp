#include "pch.h"
#include <iostream>

#include <WinSock2.h> // for socket
#include <MSWSock.h> // for AcceptEx
#include <ws2tcpip.h> // for inet_ntop
#pragma comment(lib, "ws2_32.lib") // Winsock Library

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 0;
		cout << "WSAStartup failed" << endl;
	}
	
	SOCKET clientSocket = ::socket(AF_INET,	SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "socket failed : " << errCode << endl;
		return 0;
	}

	SOCKADDR_IN serverAddr; 
	::memset(&serverAddr, 0, sizeof(serverAddr)); 
	serverAddr.sin_family = AF_INET; 
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); 
	serverAddr.sin_port = ::htons(7777); 

	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "connect failed : " << errCode << endl;
		return 0;
	}
	 
	// ------------------------------------
	// 연결 성공!

	cout << "connect success" << endl;

	while (true)
	{
		char sendBuffer[100] = "Hello World!";

		for (int32 i = 0; i < 10; ++i)
		{
			int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
			if (resultCode == SOCKET_ERROR)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "send failed : " << errCode << endl;
				return 0;
			}
		}

		cout << "send success Data Len : " << sizeof(sendBuffer) << endl;

		/*char recvBuffer[1000];
		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLen <= 0)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "recv failed : " << errCode << endl;
			return 0;
		}

		cout << "recv success Data : " << recvBuffer << endl;
		cout << "size : " << recvLen << endl;*/

		this_thread::sleep_for(1s);
	}

	// ------------------------------------

	::closesocket(clientSocket);
	::WSACleanup();
}
