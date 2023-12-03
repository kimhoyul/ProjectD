#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

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
	}

	~CoreGlobal()
	{
		delete GThreadManager;
	}
} GCoreGlobal;
