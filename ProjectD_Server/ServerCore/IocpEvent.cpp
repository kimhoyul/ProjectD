#include "pch.h"
#include "IocpEvent.h"

/*-------------------------------------------
				  IocpEvent

	OVERLAPPED ����ϴ� ��ü���� �θ� Ŭ����
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
