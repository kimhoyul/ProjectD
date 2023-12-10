#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"

ThreadManager* GThreadManager = nullptr;
MemoryManager* GMemoryManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

/*-------------------------------------------
				 CoreGlobal

	CoreServer의 순서를 관리하기 위한 클래스
	 최상위 싱글톤 클래스로 생각하면 편하다
-------------------------------------------*/
class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GMemoryManager = new MemoryManager();
		GDeadLockProfiler = new DeadLockProfiler();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemoryManager;
		delete GDeadLockProfiler;
	}
} GCoreGlobal;
