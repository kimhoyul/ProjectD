#pragma once

class GameSession;

using GameSessionRef = shared_ptr<GameSession>;

class GameSessionManager
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Brodcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	Set<GameSessionRef> _sessions;
};

extern GameSessionManager GSessionManager;