#include "pch.h"
#include "DeadLockProfiler.h"

/*-------------------------------------------
			  DeadLockProfiler

	����� ������ ������� Ž���ϱ� ���� Ŭ����
-------------------------------------------*/
void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	int32 lockId = 0;

	auto findIt = _nameToId.find(name); // �̸��� �������� ���̵� ã�´�
	if (findIt == _nameToId.end()) // ��ã�Ҵٸ�
	{
		lockId = static_cast<int32>(_nameToId.size()); // ���ο� ���̵� �����
		_nameToId[name] = lockId; // �̸��� ���̵� �ʿ� �ִ´�
		_idToName[lockId] = name; // ���̵�� �̸��� �ʿ� �ִ´�
	}
	else // ã�Ҵٸ�
	{
		lockId = findIt->second; // �̸�(key:first)���� ã�� ���̵�(value:second)�� ���
	}

	// ��� �ִ� ���� �־��ٸ�
	if (_lockStack.empty() == false)
	{
		// ������ �߰ߵ��� ���� ���� �̶�� ����� ���� �ٽ� Ȯ��
		const int32 prevId = _lockStack.top(); // ������ ���� ���� ���̵� �����´�
		if (lockId != prevId) // ������ ���� ���� ���� ���� ���� �ٸ��ٸ�
		{
			set<int32>& history = _lockHisotry[prevId]; // ������ ���� ���� �����丮�� �����´�
			if (history.find(lockId) == history.end()) // ������ ���� ���� �����丮�� ���� ���� ���� ���ٸ�
			{
				history.insert(lockId); // ������ ���� ���� �����丮�� ���� ���� ���� �ִ´�
				CheckCycle(); // ����Ŭ�� ������� Ȯ���Ѵ�
			}

		}
	}
	
	_lockStack.push(lockId); // ���� ��´�
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty()) // ���� ���� �ʾҴٸ�
		CRASH("MULTIPLE_UNLOCK_CRASH"); // ��� ũ����

	int32 lockId = _nameToId[name]; // �̸��� �������� ���̵� ã�´�
	if (_lockStack.top() != lockId) // ���� Ǯ���� �ϴ� ��	�� ���̵�� ������ ���� ���� ���̵� �ٸ��ٸ�
		CRASH("INVALID_UNLOCK_ORDER"); // ��� ũ����

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size()); // ���� ������ �����´�
	_discoveredOrder = vector<int32>(lockCount, -1); // ���(��)�� �迭�� ���� ���� ��ŭ���� �ʱ�ȭ�Ѵ�
	_discoveredCount = 0; // ��尡 �߰ߵ� ���� ������ ���� ī��Ʈ�� 0���� �ʱ�ȭ�Ѵ�
	_finished = vector<bool>(lockCount, false); // Dfs(i)�� Ž���� �������� ���θ� ����ϴ� �迭�� ���� ���� ��ŭ���� �ʱ�ȭ�Ѵ�
	_parent = vector<int32>(lockCount, -1); // Dfs(i)���� i�� �θ� ��带 ����ϴ� �迭�� ���� ���� ��ŭ���� �ʱ�ȭ�Ѵ�

	for (int32 lockId = 0; lockId < lockCount; lockId++) // ���� ���� ��ŭ �ݺ��Ѵ�
		Dfs(lockId); // Dfs(i)�� ȣ���Ѵ�

	// ������ �������� �����Ѵ�.
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)	// ��尡 �߰ߵ� ������ �̹� ��ϵǾ� �ִٸ�
		return; // �� �̻� Ž������ �ʴ´�

	_discoveredOrder[here] = _discoveredCount++; // ��尡 �߰ߵ� ������ ����Ѵ�

	// ��� ������ ��带 ��ȸ�Ѵ�.
	auto findIt = _lockHisotry.find(here); // ���� �����丮�� �����´�
	if (findIt == _lockHisotry.end()) // ���� �����丮�� ���ٸ�
	{
		_finished[here] = true; // Ž���� �������� ����Ѵ�
		return; // �� �̻� Ž������ �ʴ´� 
	}
	
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// ���� �湮�� ���� ���ٸ� �湮�Ѵ�.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here; // �θ� ��带 ����Ѵ�
			Dfs(there); // ��� ȣ���Ѵ�
			continue;
		}

		// here�� there���� ���� �߰ߵǾ��ٸ�. there�� here�� �ļ��̴�. (������ ����)
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		// �������� �ƴϰ�. Dfs(there)�� ������ �ʾҴٸ�. there�� here�� ������. (������ ����)
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];

				if (now == there)
					break;
			}
			CRASH("DEADLOCK_DETECTED_CRASH"); // ����� ũ����"
		}
	}

	_finished[here] = true; // Ž���� �������� ����Ѵ�
}
