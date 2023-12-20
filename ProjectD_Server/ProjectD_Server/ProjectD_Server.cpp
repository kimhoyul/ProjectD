#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include <WinSock2.h> // for socket
#include <MSWSock.h> // for AcceptEx
#include <ws2tcpip.h> // for inet_ntop
#pragma comment(lib, "ws2_32.lib") // Winsock Library

#include "Memory.h"

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
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT,
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	int32 type = 0; // IO_TYPE
};

void WorkerThreadMain(HANDLE iocpHandle)
{
	while (true)
	{
		// CP 결과 처리 감시
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;

		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, (ULONG_PTR*)&session, 
			(LPOVERLAPPED*)&overlappedEx, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
		{
			continue;
		}

		ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);

		cout << "[Recv Data IOCP = " << bytesTransferred << endl;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFFER_SIZE;

		DWORD recvLen = 0;
		DWORD flags = 0;

		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	}
}

int main()
{	
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		HandleError("socket()");
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
	// - 비동기 입출력 완료되면, 쓰레드마다 있는 APC 큐에 일감이 쌓임
	// - Alertable Wait 상태로 들어가서 APC 큐 비우기 (콜백 함수)
	// 단점) APC 큐가 쓰레드 마다 있어, 일감 분배가 어려움
	// 단점) Alertable Wait 자체도 조금 부담!

	// IOCP 모델 (Completion Port)
	// - APC 대신 Completion Port 라는 것을 사용 (쓰레드 마다가 아닌 커널에서 관리 = 전역 큐 느낌)
	// - Alertable Wait 대시 CP 결과 처리를 GetQueuedCompletionStatus() 함수로 처리
	// - 멀티쓰레드 친화적 환경
	
	// CreateIoComplitionPort() : 컴플리션 포트 생성 및 등록
	// GetQueuedCompletionStatus() : 결과처리 감시

	vector<Session*> sessionManager;

	// CP 생성
	/// FileHandle : 최초 생성시 INVALID_HANDLE_VALUE
	/// ExistingCompletionPort : 최초 생성시 NULL 
	/// CompletionKey : 최초 생성시 0
	/// NumberOfConcurrentThreads : 최초 생성시 0
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread 생성
	for (int32 i = 0; i < 5; i++)
		GThreadManager->Launch([=]() { WorkerThreadMain(iocpHandle); });

	//Main Thread = Accept 담당
	
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			HandleError("accept()");
			return 0;
		}

		Session* session = xnew<Session>();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		cout << "Client Connected!" << endl;

		// 소켓을 CP에 관찰 대상으로 등록
		/// FileHandle : 관찰 대상 소켓 = (HANDLE)clientSocket !!(HANDLE)로 캐스팅 해야함!!
		/// ExistingCompletionPort : 관찰 대상 소켓을 등록할 CP = iocpHandle
		/// CompletionKey : 관찰 대상 소켓을 등록할 CP에서 구분할 키값 = (ULONG_PTR)session !!(ULONG_PTR)로 캐스팅 해야함!!
		/// NumberOfConcurrentThreads : 관찰 대상 소켓을 등록할 CP에서 사용할 쓰레드 수 = 0 (최대 생성시 0)
		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, (ULONG_PTR)session, 0);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFFER_SIZE;

		DWORD recvLen = 0;
		DWORD flags = 0;

		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;

		/// s : 소켓
		/// lpBuffers : 데이터를 받을 버퍼	
		/// dwBufferCount : 버퍼 개수
		/// lpNumberOfBytesRecvd : 받은 데이터 크기
		/// lpFlags : 플래그
		/// lpOverlapped : 비동기 입출력 함수 호출 시 넘겨준 WSAOVERLAPPED 구조체의 주소
		/// lpCompletionRoutine : 비동기 입출력 완료 시 호출할 함수의 주소
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);

		/*Session* s = sessionManager.back();
		sessionManager.pop_back();
		xdelete(s);*/
	}

	GThreadManager->Join();

	// 윈속 종료
	::WSACleanup();
}