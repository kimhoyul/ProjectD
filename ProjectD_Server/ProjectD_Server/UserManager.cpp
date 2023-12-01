#include "pch.h"
#include "UserManager.h"
#include "AccountManager.h"

void UserManager::ProcessSave()
{
	// 락의 순서를 맞춰주면 해결 데드락 해결 가능
	// Account lock -> user lock 순서로 맞춰주면 해결

	// Account lock
	AccountManager::Instance()->GetAccount(100);

	// User lock
	lock_guard<mutex> guard(_mutex);

	// TODO
}
