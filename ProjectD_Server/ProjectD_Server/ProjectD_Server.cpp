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
	WSAOVERLAPPED overlapped = {};
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

	// 1) 비동기 입출력 소켓
	// 2) WSABUF 배얄의 시작 주소 + 개수
	// 3) 보내고 / 받은 바이트 수
	// 4) 상세 옵션 0
	// 5) WSAOVERLAPPED 구조체의 시작 주소
	// 6) 입출력 완료되면 OS가 호출할 콜백 함수
	// WSARecv
	// WSASend
	// Scatter, Gather
	// WSASend()
	// WSARecv()

	// 1) 비동기 입출력 소켓
	// 2) overlapped 구조체를
	// 3) 전송된 바이트 수
	// 4) 비동기 입출력 작업이 끝날때까지 대기 할지?
	// 5) false
	// 6) 비동기 입출력 작업 관련 부가 정보, 거의 사용 안함
	// WSAGetOverlappedResult

	// Overlapped 모델(이벤트 기반)
	// - 비동기 입출력 지원하는 소켓 생성 + 통지 받기 위한 이벤트 객체 생성
	// - 비동기 입출력 함수 호출 (1에서 만든 이벤트 객체를 같이 넘겨줌)
	// - 비동기 작업이 완료되지 않으면, WSA_IO_PENDING 반환
	// - 운영체제는 이벤트 객체를 signaled 상태로 만들고 완료상태를 통지
	// - WSAWaitForMultipleEvents 함수를 통해 이벤트 객체의 시그널 상태 판별
	// - WSAGetOverlappedResult 함수를 통해 비동기 작업 결과 얻기 및 데이터 처리

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != listenSocket)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			HandleError("accept()");
			return 0;
		}

		Session session = Session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << "Client Connected!" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFFER_SIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;

			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, nullptr) == SOCKET_ERROR)
			{
				// 지연상태
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
					::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
				}
				else
				{
					HandleError("WSARecv()");
					break;
				}
			}

			cout << "Recv Data Len : " << recvLen << " | Recv Data : " << session.recvBuffer << endl;
		}

		::closesocket(clientSocket);
		::WSACloseEvent(wsaEvent);
	}

	// 윈속 종료
	::WSACleanup();
}