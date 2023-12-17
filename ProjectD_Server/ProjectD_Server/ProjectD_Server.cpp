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

int main()
{	
	// 윈속 초기화(ws2_32.dll을 초기화)
	// 관련 정보가 wsaData에 저장됨
	WSAData wsaData;

	// 성공하면 0을 반환
	// WSAStartup을 성공적으로 호출한 후에만 추가 Windows 소켓 함수를 실행할 수 있음
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 0;
		cout << "WSAStartup failed" << endl;
	}

	// 소켓 생성
	// 성공하면 소켓 핸들을 반환
	SOCKET listenSocket = ::socket
	(
		AF_INET,		// 주소 체계 (Address Family : IPv4)
		SOCK_STREAM,	// 통신 방식 (Type : SOCK_STREAM - TCP)
		0				// 프로토콜 (protocal : 0 - Auto)
	);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "socket failed : " << errCode << endl;
		return 0;
	}

	SOCKADDR_IN serverAddr; // 서버 주소 IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr)); // 0으로 초기화
	serverAddr.sin_family = AF_INET; // IPv4
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // IP 주소 (Host To Network Long)
	serverAddr.sin_port = ::htons(7777); // Port 번호 (Host To Network Short)

	// 소켓에 주소 정보를 할당
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "bind failed : " << errCode << endl;
		return 0;
	}

	// 소켓을 연결 요청 받을 수 있는 상태로 변경
	if (::listen(listenSocket, 10) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "listen failed : " << errCode << endl;
		return 0;
	}
	
	// --------------------------------
	// 연결 대기 상태
	while (true)
	{
		SOCKADDR_IN clientAddr;
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "accept failed : " << errCode << endl;
			return 0;
		}

		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "client connected! IP : " << ipAddress << endl;

		// TODO
	}

	// --------------------------------

	// 윈속 종료
	::WSACleanup();
}