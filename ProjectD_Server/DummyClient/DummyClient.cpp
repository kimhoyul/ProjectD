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
	
	SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("Socket failed");
		return 0;
	}

	SOCKADDR_IN serverAddr; 
	::memset(&serverAddr, 0, sizeof(serverAddr)); 
	serverAddr.sin_family = AF_INET; 
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777); 
		 
	// Connected UDP
	::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
		 
	// ------------------------------------

	while (true)
	{
		char sendBuffer[100] = "Hello World!";

		// Unconnected UDP
		/*int32 errCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0,
			(SOCKADDR*)&serverAddr, sizeof(serverAddr));*/

		// Connected UDP
		int32 errCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);

		if (errCode == SOCKET_ERROR)
		{
			HandleError("SendTo failed");
			return 0;
		}

		cout << "send success Data Len : " << sizeof(sendBuffer) << endl;

		SOCKADDR_IN recvAddr;
		::memset(&recvAddr, 0, sizeof(recvAddr));
		int32 addrLen = sizeof(recvAddr);

		char recvBuffer[1000];

		// Unconnected UDP
		/*int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0,
			(SOCKADDR*)&recvAddr, &addrLen);*/

		// Connected UDP
		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLen <= 0)
		{
			HandleError("Recvfrom failed");
			return 0;
		}

		cout << "Recv Data : " << recvBuffer << endl;
		cout << "Recv Data Len : " << recvLen << endl;

		this_thread::sleep_for(1s);
	}

	// ------------------------------------

	::closesocket(clientSocket);
	::WSACleanup();
}
