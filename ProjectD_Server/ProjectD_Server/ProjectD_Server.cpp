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

const int32 BUFFER_SIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFFER_SIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0;
};

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

	// Overlapped IO (비동기 + 논블로킹)
	// - Overlapped 함수를 건다 (WSARecv, WSASend, AcceptEX, ConnectEx)
	// - Overlapped 함수가 성공했는지 확인 후
	// - 성공했다면 결과를 얻어서 처리
	// - 실패했다면 사유 확인

	// WSASend, WSARecv, AcceptEx, ConnectEx



	// 윈속 종료
	::WSACleanup();
}