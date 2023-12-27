#pragma once
#include <stack>
#include <map>
#include <vector>

/*-------------------------------------------
			  DeadLockProfiler

	디버그 상태의 데드락을 탐지하기 위한 클래스
-------------------------------------------*/
class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();
	
private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32> _nameToId;
	unordered_map<int32, const char*> _idToName;
	map<int32, set<int32>> _lockHisotry;

	Mutex _lock;

private:
	vector<int32> _discoveredOrder; // 노드가 발견된 순서를 기록하는 배열
	int32 _discoveredCount = 0; // 노드가 발견된 순서 추적을 위한 카운트
	vector<bool> _finished; // Dfs(i)의 탐색이 끝났는지 여부를 기록하는 배열
	vector<int32> _parent; // Dfs(i)에서 i의 부모 노드를 기록하는 배열
};

