#pragma once
#include "JobSerializer.h"

class Room : public JobSerializer
{
public:
	// 싱글쓰레드 환경 처럼 사용
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);
	
public:
	// 멀티쓰레드 환경에서 사용
	virtual void FlushJob() override;

private:
	map<uint64, PlayerRef> _players;
};

extern shared_ptr<Room> GRoom;
