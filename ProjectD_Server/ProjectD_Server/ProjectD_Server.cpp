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

	// Select 모델 
	// 소켓 함수 호출이 성공할 시점을 미리 알 수 있다.
	// 문제 상황)
	// 수신버퍼에 데이터가 없는데, read 한다거나
	// 송신버퍼가 꽉 찼는데, write 한다거나
	// - 블로킹 소켓 : 조건이 만족되지 않아서 블로킹 되는 상황 예방
	// - 논 블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방

	// socket set : 관찰 대상이 되는 소켓들을 모아놓은 집합
	// 1) 읽기[ ] 쓰기[ ] 예외(OOB)[ ] 관찰 대상 등록
	// Out Of Band는 sand() 마지막에 인자 MSG_OOB로 보내는 특별한 데이터
	// 받는 쪽에서도 recv() 할때 OOB 세팅을 해야 읽을 수 있음
	// 2) select(readSet, writeSet, exceptSet); 호출 -> 관찰 시작
	// 3) select()는 관찰 대상 중 하나라도 조건이 만족되면 리턴 -> 낙오자는 알아서 제거됨
	// 4) 남은 소켓 체크해서 진행

	// fd_set set: 소켓을 관찰하기 위한 자료구조
	// FD_ZERO() : ex) FD_ZERO(set) : fd_set 초기화
	// FD_SET() : ex) FD_SET(socket, &set) : fd_set에 socket을 추가
	// FD_CLR : ex) FD_CLR(socket, &set) : fd_set에서 socket을 제거
	// FD_ISSET : ex) FD_ISSET(socket, &set) : fd_set에 socket이 없으면 0을 리턴

	vector<Session> sessions;
	sessions.reserve(100);

	fd_set readSet;
	fd_set writeSet;

	while (true)
	{
		// 소켓 셋 초기화
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		// Listen Socket 등록
		FD_SET(listenSocket, &readSet);

		// 소켓 등록
		for (Session& session : sessions)
		{
			// 에코서버를 위한 코드 : recvBytes 가 sendBytes 보다 크면 보낼게 남아있는 것
			if (session.recvBytes <= session.sendBytes)
				FD_SET(session.socket, &readSet);
			else
				FD_SET(session.socket, &writeSet);
		}

		// [옵션] 마지막 인자 TIMEOUT 설정 가능
		timeval timeout;
		timeout.tv_sec;		// 초
		timeout.tv_usec;	// 마이크로초
		int32 retVal = ::select(0, &readSet, &writeSet, nullptr, nullptr);
		if (retVal == SOCKET_ERROR)
		{
			HandleError("select()");
			break;
		}

		// Listen Socket 체크
		if (FD_ISSET(listenSocket, &readSet))
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected! : " << endl;
				sessions.push_back(Session{clientSocket});
			}
		}

		for (Session& s : sessions)
		{
			// Read 체크
			if (FD_ISSET(s.socket, &readSet))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFFER_SIZE, 0);
				if (recvLen <= 0)
				{
					// TODO : sessions 에서 제거
					
					continue;
				}

				s.recvBytes = recvLen;
			}

			// Write 체크
			if (FD_ISSET(s.socket, &writeSet))
			{
				// 일부만 보내질 것을 가정하여 sendBytes를 사용
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					// TODO : sessions 에서 제거

					continue;
				}

				s.sendBytes += sendLen;
				if (s.sendBytes == s.recvBytes)
				{
					s.sendBytes = 0;
					s.recvBytes = 0;
				}
			}
		}
	}

	// 윈속 종료
	::WSACleanup();
}