#pragma once

/*-------------------------------------------
				  IocpObject

     IOCP에서 사용하는 객체들의 부모 클래스
-------------------------------------------*/
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};


/*-------------------------------------------
				  IocpCore
				  
			  IOCP의 핵심 부분과 
      Complication Port를 관리 하는 클래스
-------------------------------------------*/
class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle() { return _iocpHandle; }

	// 관찰 대상 소켓 등록 함수
	bool Register(class IocpObject* iocpObject);
	// Worker Thread 가 일감이 있는지 확인 하는 함수
	bool Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE _iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;