#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

/*-------------------------------------------
				 CoreGlobal

	CoreServer의 순서를 관리하기 위한 클래스
	 최상위 싱글톤 클래스로 생각하면 편하다
-------------------------------------------*/
CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
}
