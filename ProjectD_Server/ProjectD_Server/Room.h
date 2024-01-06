#pragma once
#include "Job.h"

class Room
{
public:
	// 싱글쓰레드 환경 처럼 사용
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);
	
public:
	// 멀티쓰레드 환경에서 사용
	void PushJob(JobRef job) { _jobs.Push(job); }
	void FlushJobs();

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*memFunc)(Args...), Args... args)
	{
		auto job = MakeShared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
		 _jobs.Push(job);
	}

private:
	map<uint64, PlayerRef> _players;
	JobQueue _jobs;
};

extern Room GRoom;
