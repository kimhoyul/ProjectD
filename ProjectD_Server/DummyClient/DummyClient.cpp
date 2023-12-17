#include "pch.h"
#include <iostream>

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
	SOCKET clientSocket = ::socket
		(
			AF_INET,		// 주소 체계 (Address Family : IPv4)
			SOCK_STREAM,	// 통신 방식 (Type : SOCK_STREAM - TCP)
			0				// 프로토콜 (protocal : 0 - Auto)
		);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "socket failed : " << errCode << endl;
		return 0;
	}

	SOCKADDR_IN serverAddr; // 서버 주소 IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr)); // 0으로 초기화
	serverAddr.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // IP 주소 (신식)
	// little endian / big endian 이슈 때문에 htons를 사용
	serverAddr.sin_port = ::htons(7777); // Port 번호 (Host To Network Short)

	// 서버에 접속
	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "connect failed : " << errCode << endl;
		return 0;
	}

	// --------------------------------
	// 연결 성공 !
	cout << "connect success" << endl;

	while (true)
	{
		//TODO: 서버로부터 데이터를 받는다.

		this_thread::sleep_for(1s);
	}
	// --------------------------------

	// 소켓 닫기
	::closesocket(clientSocket);

	// 윈속 종료
	::WSACleanup();
}
