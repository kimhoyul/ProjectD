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
	WSAOVERLAPPED overlapped = {};
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFFER_SIZE] = {};
	int32 recvBytes = 0;
};

/// <summary>
/// 완료 루틴 (콜백 함수)
/// </summary>
/// <param name="dwError">오류 발생시 0 아닌 값이 리턴 : error</param>
/// <param name="cbTransferred">전송 바이트 수 : recvLen</param>
/// <param name="lpOverlapped">비동기 입출력 함수 호출 시 넘겨준 WSAOVERLAPPED 구조체의 주소 : Overlapped</param>
/// <param name="dwFlags">사용안함 0 : flags</param>
void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED Overlapped, DWORD flags)
{
	cout << "[DATA RecvCallback]" << endl;
	cout << "Recv Data Len : " << recvLen << endl;

	Session* session = (Session*)Overlapped;

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
	// set non-blocking : 비동기 입출력을 소켓으로 변경
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

	// Overlapped 모델(Complition Routine : 콜백 기반)
	// - 비동기 입출력 지원하는 소켓 생성
	// - 비동기 입출력 함수 호출 (완료 루틴(콜백함수)의 시작 주소를 넘겨준다)
	// - 비동기 작업이 완료되지 않으면, WSA_IO_PENDING 오류코드 반환
	// - 비동기 IO 완료되면, 운영체제는 완료 루틴 호출
	// 단,비동기 입출력 함수 호출한 쓰레드를 -> Alertable Wait 상태로 만들어 줘야 완료 루틴이 호출 된다.
	//  ex) WaitForSingleObjectEx, WaitForMultipleObjectEx, SleepEx, WSAWaitForMultipleEvents
	// - 완료 루틴 호출이 끝나면, 쓰레드는 Alertable Wait 상태에서 빠져나온다.

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

		cout << "Client Connected!" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFFER_SIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;

			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
			{
				// 지연상태
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Alertable Wait 상태로 만들어 준다.
					::SleepEx(INFINITE, TRUE); // 호출할 완료 루틴(콜백함수) 가 있는지 확인후 무한 대기
					// 완료 루틴이 여러개 등록되어있다면 먼저 등록된 완료 루틴부터 호출
					// 모든 완료 루틴이 호출되 소모되어야 쓰레드는 빠져나온다.
					//::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE);
				}
				else
				{
					HandleError("WSARecv()");
					break;
				}
			}
			else
			{
				cout << "Recv Data Len : " << recvLen << " | Recv Data : " << session.recvBuffer << endl;
			}
		}

		::closesocket(clientSocket);
		::WSACloseEvent(wsaEvent);
	}

	// 윈속 종료
	::WSACleanup();
}