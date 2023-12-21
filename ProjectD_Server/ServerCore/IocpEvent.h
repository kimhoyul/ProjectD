#pragma once

enum class EventType : uint8
{
	Connect,
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
	EventType GetType() { return _type; }
	
protected:
	EventType _type;
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
				  AcceptEvent

			각 타입별 이벤트 클래스
-------------------------------------------*/
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

private:
	// TODO
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
};