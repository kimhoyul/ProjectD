#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv, // 0byte recv : Recv를 하기 전에 미리 RecvBuf를 채워놓는다.
	Recv,
	Send,
};

/*-------------------------------------------
				  IocpEvent

    OVERLAPPED 사용하는 객체들의 부모 클래스
-------------------------------------------*/
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType eventType);

	void Init();
	
public:
	EventType eventType;
	IocpObjectRef owner;
};

/*-------------------------------------------
				 ConnectEvent

			각 타입별 이벤트 클래스
-------------------------------------------*/
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*-------------------------------------------
				DisconnectEvent

			각 타입별 이벤트 클래스
-------------------------------------------*/
class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) { }
};

/*-------------------------------------------
				  AcceptEvent

			각 타입별 이벤트 클래스
-------------------------------------------*/
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

public:
	SessionRef session = nullptr;
};

/*-------------------------------------------
			      RecvEvent

			각 타입별 이벤트 클래스
-------------------------------------------*/
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*-------------------------------------------
				  SendEvent

			각 타입별 이벤트 클래스
-------------------------------------------*/
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }

	// TEMP : 임시 샌드 버퍼
	vector<BYTE> buffer;
};