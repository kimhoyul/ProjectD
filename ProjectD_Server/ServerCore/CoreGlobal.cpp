#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

/*-------------------------------------------
				 CoreGlobal

	CoreServer�� ������ �����ϱ� ���� Ŭ����
	 �ֻ��� �̱��� Ŭ������ �����ϸ� ���ϴ�
-------------------------------------------*/
CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
}
