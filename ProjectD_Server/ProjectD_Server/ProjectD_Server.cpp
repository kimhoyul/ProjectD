#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

class GameSession : public Session
{
public:
	~GameSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		// echo
		cout << "On Recv Len = " << len << endl;

		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
		sendBuffer->CopyData(buffer, len);

		Send(sendBuffer);
	
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