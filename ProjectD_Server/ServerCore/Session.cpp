#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"

/*-------------------------------------------
				  Session

  클라이언트 소켓의 모든 정보를 가지고 있는 클래스
-------------------------------------------*/
Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	// TODO : 이벤트 처리
}
