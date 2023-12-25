#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv, // 0byte recv : Recv�� �ϱ� ���� �̸� RecvBuf�� ä�����´�.
	Recv,
	Send,
};

/*-------------------------------------------
				  IocpEvent

    OVERLAPPED ����ϴ� ��ü���� �θ� Ŭ����
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

			�� Ÿ�Ժ� �̺�Ʈ Ŭ����
-------------------------------------------*/
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*-------------------------------------------
				DisconnectEvent

			�� Ÿ�Ժ� �̺�Ʈ Ŭ����
-------------------------------------------*/
class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) { }
};

/*-------------------------------------------
				  AcceptEvent

			�� Ÿ�Ժ� �̺�Ʈ Ŭ����
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

			�� Ÿ�Ժ� �̺�Ʈ Ŭ����
-------------------------------------------*/
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*-------------------------------------------
				  SendEvent

			�� Ÿ�Ժ� �̺�Ʈ Ŭ����
-------------------------------------------*/
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }

	// TEMP : �ӽ� ���� ����
	vector<BYTE> buffer;
};