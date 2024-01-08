#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"

ThreadManager*		GThreadManager = nullptr;
MemoryManager*		GMemoryManager = nullptr;
SendBufferManager*	GSendBufferManager = nullptr;
GlobalQueue*		GGlobalQueue = nullptr;

DeadLockProfiler*	GDeadLockProfiler = nullptr;

/*-------------------------------------------
				 CoreGlobal

	CoreServer�� ������ �����ϱ� ���� Ŭ����
	 �ֻ��� �̱��� Ŭ������ �����ϸ� ���ϴ�
-------------------------------------------*/
class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GMemoryManager = new MemoryManager();
		GSendBufferManager = new SendBufferManager();
		GGlobalQueue = new GlobalQueue();
		GDeadLockProfiler = new DeadLockProfiler();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemoryManager;
		delete GSendBufferManager;
		delete GGlobalQueue;
		delete GDeadLockProfiler;
		SocketUtils::Clear();
	}
} GCoreGlobal;
