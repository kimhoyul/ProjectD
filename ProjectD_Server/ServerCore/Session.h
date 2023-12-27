#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

/*-------------------------------------------
				  Session

  Ŭ���̾�Ʈ ������ ��� ������ ������ �ִ� Ŭ����
-------------------------------------------*/
class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, //64KB
	};

public:
	Session();
	virtual ~Session();

public:
	/* �ܺο��� ����ϴ� �Լ� */
	void Send(SendBufferRef sendbuffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	shared_ptr<Service> GetService() { return _service.lock(); }
	void SetService(shared_ptr<Service> service) { _service = service; }

public:
	/* ���� ���� */
	void SetNetAddress(const NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }
	bool IsConnected() { return _connected; }
	SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	/* �������̽� ���� */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/* ���� ���� */
	bool RegisterConnect();
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();
	
	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 numOfBytes);

	void HandleError(int32 errorCode);

protected:
	/* ������ �ڵ忡�� �������ؼ� ����� �Լ� */
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
	virtual void OnDisconnected() {}

private:
	weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false; // ���� ���� Ȯ�ο�

private:
	USE_LOCK;

	/* ���� ���� */
	RecvBuffer _recvBuffer;
	
	/* �۽� ���� */
	// RegisterSend�� �������̾ ���� ���� WSASend�� �Ҽ� ������ �����ص� ť
	Queue<SendBufferRef> _sendQueue;
	// RegisterSend�� ���������� ���θ� �Ǵ��ϴ� ����
	Atomic<bool> _sendRegister = false;

private:  
	/* IocpEvenet ������ �ϱ����� ��� ���� */
	ConnectEvent	_connectEvent;
	DisconnectEvent	_disconnectEvent;
	RecvEvent		_recvEvent;
	SendEvent		_sendEvent;


};

