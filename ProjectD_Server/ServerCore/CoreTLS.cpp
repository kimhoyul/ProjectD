#include "pch.h"
#include "CoreTLS.h"

thread_local uint32				LThreadId = 0; // 직접적으로 관리할 쓰레드 ID
thread_local uint64				LEndTickCount = 0; 
thread_local std::stack<int32>	LLockStack;	// 각 쓰레드별(TLS) 사용할 락 스택
thread_local SendBufferChunkRef	LSendBufferChunk; // 각 쓰레드별(TLS) 사용할 SendBufferChunk
thread_local JobQueue*			LCurrentJobQueue = nullptr; // 쓰레드가 어떤 잡을 처리하고 있는지 확인하는 용도