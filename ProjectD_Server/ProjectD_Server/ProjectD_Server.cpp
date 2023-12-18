#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

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

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		HandleError("socket()");
		return 0;
	}

	u_long on = 1;
	// set non-blocking
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		HandleError("ioctlsocket()");
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		HandleError("bind()");
		return 0;
	}
	
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		HandleError("listen()");
		return 0;
	}

	cout << "Accepting..." << endl;

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);

	while (true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			// 원래 블록했어야 했지만 논블록으로 바꿔서 INVALID_SOCKET 빠질 수 있음
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			HandleError("accept()");
			break;
		}

		cout << "Client Connected" << endl;

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
				cout << "Client Disconnected" << endl;
				break;
			}

			cout << "Recv Data Len : " << recvLen << " | Recv Data : "  << recvBuffer << endl;

			while (true)
			{
				int32 sendLen = ::send(clientSocket, recvBuffer, recvLen, 0);
				if (sendLen == SOCKET_ERROR)
				{
					// 원래 블록했어야 했지만 논블록으로 바꿔서 SOCKET_ERROR 빠질 수 있음
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
						continue;

					HandleError("send()");
					break;
				}
				
				cout << "Send Data Len : " << sendLen << " | Send Data : " << recvBuffer << endl;
				break;
			}
		}
	}

	::WSACleanup();
}