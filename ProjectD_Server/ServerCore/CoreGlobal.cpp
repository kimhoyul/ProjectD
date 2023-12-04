#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

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
		GDeadLockProfiler = new DeadLockProfiler();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GDeadLockProfiler;
	}
} GCoreGlobal;
