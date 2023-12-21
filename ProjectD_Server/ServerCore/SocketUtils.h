#pragma once
#include "NetAddress.h"

/*-------------------------------------------
				 SocketUtils

		  윈속 초기화 및 비동기 함수들의 
			포인터를 얻어오는 클래스

  또한, 각종 소켓 옵션과 함수들을 랩핑하여 사용의 
		   편의성을 위해 제작된 클래스
-------------------------------------------*/
class SocketUtils
{
public:
	// 함수 포인터
	// ConnectEx / DisconnectEX / AcceptEX 는 Windows Sockets API (Winsock)의 확장 함수로, 
	// 비동기적으로 소켓 연결을 수행함. 
	// 이 함수는 Winsock API에 직접 포함되어 있지 않기 때문에, 
	// WSAIoctl을 사용하여 런타임에 해당 함수의 주소를 얻어야 함.
	// 얻어온 함수의 주소를 저장할 변수
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

public:
	static void Init();
	static void Clear();

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static SOCKET CreateSocket(); // 소켓 생성

	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	static bool Bind(SOCKET socket, NetAddress netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void Close(SOCKET& socket);
};

// SetSocketOption 을 편하게 세팅하기 위한 템플릿 함수
template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}
