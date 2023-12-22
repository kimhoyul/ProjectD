#pragma once

/*-------------------------------------------
				  IocpObject

     IOCP���� ����ϴ� ��ü���� �θ� Ŭ����
-------------------------------------------*/
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};


/*-------------------------------------------
				  IocpCore
				  
			  IOCP�� �ٽ� �κа� 
      Complication Port�� ���� �ϴ� Ŭ����
-------------------------------------------*/
class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle() { return _iocpHandle; }

	// ���� ��� ���� ��� �Լ�
	bool Register(class IocpObject* iocpObject);
	// Worker Thread �� �ϰ��� �ִ��� Ȯ�� �ϴ� �Լ�
	bool Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE _iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;