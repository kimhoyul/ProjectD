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

	// WSAEventSelect 모델 (WSAEventSelect 함수가 핵심이 되는 모델)
	// 소켓과 관련된 네트워크 이벤트를 [이벤트 객체]를 통해 감지
	// 이벤트를 통해서 통지를 받음
	
	// 이벤트 객체 관련 함수들
	// ::WSACreateEvent() : 이벤트 객체 생성
	// ::WSAEventSelect() : 소켓과 이벤트 객체 연결
	// ::WSAWaitForMultipleEvents() : 이벤트 객체가 신호를 보낼 때까지 대기
	// ::WSAEnumNetworkEvents() : 이벤트 객체가 신호를 보낸 이벤트 확인
	// ::WSAResetEvent() : 이벤트 객체를 초기화
	// ::WSACloseEvent() : 이벤트 객체를 닫음

	// 소켓과 이벤트 객체 연결
	// WSAEventSelect(socket, event, networkEvents);
	// - 관찰할 네트워크 이벤트	
	// - FD_ACCEPT : 접속한 클라가 있음 accept
	// - FD_READ : 데이터 수신 가능 recv, redvfrom
	// - FD_WRITE : 데이터 송신 가능 send, sendto
	// - FD_CLOSE : 상대가 접속 종료
	// - FD_CONNECT : 연결 요청 완료 
	// - FD_OOB : 긴급 데이터 수신 가능

	// 주의 사항
	// WSAEventSelect 함수를 호출하면, 해당 소켓은 자동으로 논블록 모드로 변경됨
	// accept 함수가 리턴하는 소켓은 listenSocket과 동일한 속성을 가짐
	// - 따라서 clientSocket은 FD_READ, FD_WRITE 등을 다시 등록 필요
	// 드물게 WSAEWOLDBLOCK 에러가 발생할 수 있음
	// - 이벤트 발생 시, 적절한 소켓 함수 호출해야 함
	// - 아니면 다음 번에는 동일 네트워크 이벤트가 발생 X
	// ex) FD_READ 발생 시, recv 함수 호출, 안하면 FD_READ 발생 X

	// 이벤트 개수, 이벤트 객체 배열
	// 모두 기다릴지 아니면 하나라도 신호가 오면 리턴할지
	// 이벤트 객체 신호를 기다릴 시간
	// 지금은 사용하지 않음
	// 리턴되는 값은 이벤트 객체 배열의 인덱스
	// WSAWaitForMultipleEvents 함수

	// 1) socket
	// 2) 이벤트 객체 : 소켓과 연동된 이벤트 객체 핸들을 넘겨주면, 이벤트 객체를 Non-signal 상태로 변경
	// 3) 네트워크 이벤트 : 이벤트 객체가 신호를 보낼 때까지 대기
	// WSAEnumNetworkEvents 함수

	vector<WSAEVENT> wsaEvents;
	vector<Session> sessions;
	sessions.reserve(100);

	WSAEVENT listenEvent = ::WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sessions.push_back(Session{ listenSocket });
	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		HandleError("WSAEventSelect()");
		return 0;
	}

	while (true)
	{
		int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED)
			continue;

		index -= WSA_WAIT_EVENT_0;

		// WSAEnumNetworkEvents 에서 아래 함수와 동일한 기능을 수행함
		//::WSAResetEvent(wsaEvents[index]);

		WSANETWORKEVENTS networkEvents;
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
		{
			HandleError("WSAEnumNetworkEvents()");
			return 0;
		}

		// Listner 소켓 체크
		// 엑셉트 신호인지, 클로즈 신호인지 확인
		if (networkEvents.lNetworkEvents & FD_ACCEPT) // 비트 플래그 체크
		{
			// 엑셉트 신호에 에러가 있는지 확인
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				HandleError("FD_ACCEPT_BIT");
				return 0;
			}

			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected!" << endl;

				WSAEVENT clientEvent = ::WSACreateEvent();
				wsaEvents.push_back(clientEvent);
				sessions.push_back(Session{ clientSocket });
				if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
				{
					HandleError("WSAEventSelect()");
					return 0;
				}
			}
		}

		// Client Session 소켓 체크
		if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
		{
			// FD_READ 에러가 있는지 확인
			if (networkEvents.iErrorCode[FD_READ] != 0)
			{
				HandleError("FD_READ_BIT");
				return 0;
			}

			// FD_WRITE 에러가 있는지 확인
			if (networkEvents.iErrorCode[FD_READ] != 0)
			{
				HandleError("FD_READ_BIT");
				return 0;
			}


		}


	}

	// 윈속 종료
	::WSACleanup();
}