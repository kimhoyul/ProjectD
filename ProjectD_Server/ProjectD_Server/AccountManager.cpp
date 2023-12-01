#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"

void AccountManager::ProcessLogin()
{
	// Account lock
	lock_guard<mutex> guard(_mutex);

	// User lock
	UserManager::Instance()->GetUser(100);

	// TODO
}
