#include "pch.h"
#include "DeadLockProfiler.h"

/*-------------------------------------------
			  DeadLockProfiler

	디버그 상태의 데드락을 탐지하기 위한 클래스
-------------------------------------------*/
void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	int32 lockId = 0;

	auto findIt = _nameToId.find(name); // 이름을 기준으로 아이디를 찾는다
	if (findIt == _nameToId.end()) // 못찾았다면
	{
		lockId = static_cast<int32>(_nameToId.size()); // 새로운 아이디를 만든다
		_nameToId[name] = lockId; // 이름과 아이디를 맵에 넣는다
		_idToName[lockId] = name; // 아이디와 이름을 맵에 넣는다
	}
	else // 찾았다면
	{
		lockId = findIt->second; // 이름(key:first)으로 찾은 아이디(value:second)를 사용
	}

	// 잡고 있는 락이 있었다면
	if (LLockStack.empty() == false)
	{
		// 기존에 발견되지 않은 간선 이라면 데드락 여부 다시 확인
		const int32 prevId = LLockStack.top(); // 직전에 잡은 락의 아이디를 가져온다
		if (lockId != prevId) // 직전에 잡은 락과 지금 잡은 락이 다르다면
		{
			set<int32>& history = _lockHisotry[prevId]; // 이전에 잡은 락의 히스토리를 가져온다
			if (history.find(lockId) == history.end()) // 이전에 잡은 락의 히스토리에 지금 잡은 락이 없다면
			{
				history.insert(lockId); // 이전에 잡은 락의 히스토리에 지금 잡은 락을 넣는다
				CheckCycle(); // 사이클이 생겼는지 확인한다
			}

		}
	}
	
	LLockStack.push(lockId); // 락을 잡는다
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (LLockStack.empty()) // 락을 잡지 않았다면
		CRASH("MULTIPLE_UNLOCK_CRASH"); // 언락 크래시

	int32 lockId = _nameToId[name]; // 이름을 기준으로 아이디를 찾는다
	if (LLockStack.top() != lockId) // 지금 풀려고 하는 락	의 아이디와 직전에 잡은 락의 아이디가 다르다면
		CRASH("INVALID_UNLOCK_ORDER"); // 언락 크래시

	LLockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size()); // 락의 개수를 가져온다
	_discoveredOrder = vector<int32>(lockCount, -1); // 노드(락)의 배열을 락의 개수 만큼으로 초기화한다
	_discoveredCount = 0; // 노드가 발견된 순서 추적을 위한 카운트를 0으로 초기화한다
	_finished = vector<bool>(lockCount, false); // Dfs(i)의 탐색이 끝났는지 여부를 기록하는 배열을 락의 개수 만큼으로 초기화한다
	_parent = vector<int32>(lockCount, -1); // Dfs(i)에서 i의 부모 노드를 기록하는 배열을 락의 개수 만큼으로 초기화한다

	for (int32 lockId = 0; lockId < lockCount; lockId++) // 락의 개수 만큼 반복한다
		Dfs(lockId); // Dfs(i)를 호출한다

	// 연산이 끝났으면 정리한다.
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)	// 노드가 발견된 순서가 이미 기록되어 있다면
		return; // 더 이상 탐색하지 않는다

	_discoveredOrder[here] = _discoveredCount++; // 노드가 발견된 순서를 기록한다

	// 모든 인접한 노드를 순회한다.
	auto findIt = _lockHisotry.find(here); // 락의 히스토리를 가져온다
	if (findIt == _lockHisotry.end()) // 락의 히스토리가 없다면
	{
		_finished[here] = true; // 탐색이 끝났음을 기록한다
		return; // 더 이상 탐색하지 않는다 
	}
	
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// 아직 방문한 적이 업다면 방문한다.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here; // 부모 노드를 기록한다
			Dfs(there); // 재귀 호출한다
			continue;
		}

		// here가 there보다 먼저 발견되었다면. there는 here의 후손이다. (순방향 간선)
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		// 순방향이 아니고. Dfs(there)가 끝나지 않았다면. there는 here의 선조다. (역방향 간선)
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
			CRASH("DEADLOCK_DETECTED_CRASH"); // 데드락 크래시"
		}
	}

	_finished[here] = true; // 탐색이 끝났음을 기록한다
}
