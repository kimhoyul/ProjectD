#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

/*-------------------------------------------
				  Session

  클라이언트 소켓의 모든 정보를 가지고 있는 클래스
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
	/* 외부에서 사용하는 함수 */
	void Send(SendBufferRef sendbuffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	shared_ptr<Service> GetService() { return _service.lock(); }
	void SetService(shared_ptr<Service> service) { _service = service; }

public:
	/* 정보 관련 */
	void SetNetAddress(const NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }
	bool IsConnected() { return _connected; }
	SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	/* 인터페이스 구현 */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/* 전송 관련 */
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
	/* 콘텐츠 코드에서 재정의해서 사용할 함수 */
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
	virtual void OnDisconnected() {}

private:
	weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false; // 접속 상태 확인용

private:
	USE_LOCK;

	/* 수신 관련 */
	RecvBuffer _recvBuffer;
	
	/* 송신 관련 */
	// RegisterSend가 실행중이어서 내가 당장 WSASend를 할수 없을때 저장해둘 큐
	Queue<SendBufferRef> _sendQueue;
	// RegisterSend가 실행중인지 여부를 판단하는 변수
	Atomic<bool> _sendRegister = false;

private:  
	/* IocpEvenet 재사용을 하기위한 멤버 변수 */
	ConnectEvent	_connectEvent;
	DisconnectEvent	_disconnectEvent;
	RecvEvent		_recvEvent;
	SendEvent		_sendEvent;


};

