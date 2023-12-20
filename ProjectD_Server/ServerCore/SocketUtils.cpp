#include "pch.h"
#include "SocketUtils.h"

/*-------------------------------------------
				SocketUtils


-------------------------------------------*/
LPFN_CONNECTEX		SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

	/*런타임에 주소 얻어오는 API*/

}

void SocketUtils::Clear()
{
	
}

bool SocketUtils::BindWindowFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	return false;
}

SOCKET SocketUtils::CreateSocket()
{
	return SOCKET();
}
