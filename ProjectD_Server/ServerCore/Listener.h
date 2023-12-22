#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;

/*-------------------------------------------
				  Listener 

		 AcceptEvent를 관리하는 클래스
-------------------------------------------*/
class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	/* 외부에서 사용*/
	bool StartAccept(NetAddress nerAddress);
	void CloseSocket();

public:
	/* 인터페이스 구현 */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/* 수신 관련 */
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

protected:
	SOCKET _socket = INVALID_SOCKET;
	Vector<AcceptEvent*> _acceptEvents;
};

