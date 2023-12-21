#include "pch.h"
#include "IocpEvent.h"

/*-------------------------------------------
				  IocpEvent

	OVERLAPPED 사용하는 객체들의 부모 클래스
-------------------------------------------*/

IocpEvent::IocpEvent(EventType eventType)
{
	Init();
}

void IocpEvent::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
