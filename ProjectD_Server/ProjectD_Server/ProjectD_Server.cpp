#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"

class GameSession : public Session
{
public:
	virtual int32 OnRecv(BYTE* buuter, int32 len) override
	{
		// echo
		cout << "On Recv Len = " << len << endl;
		Send(buuter, len);
	
		return len;
	}

	virtual void OnSend(int32 len) override
	{
		cout << "On Send Len = " << len << endl;
	}
};

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SesseionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]() 
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}