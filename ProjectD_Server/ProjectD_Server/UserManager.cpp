#include "pch.h"
#include "UserManager.h"
#include "AccountManager.h"

void UserManager::ProcessSave()
{
	// ���� ������ �����ָ� �ذ� ����� �ذ� ����
	// Account lock -> user lock ������ �����ָ� �ذ�

	// Account lock
	AccountManager::Instance()->GetAccount(100);

	// User lock
	lock_guard<mutex> guard(_mutex);

	// TODO
}
