#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadId = 0; // 직접적으로 관리할 쓰레드 ID
thread_local std::stack<int32> LLockStack;	// 각 쓰레드별(TLS) 사용할 락 스택
thread_local SendBufferChunkRef	LSendBufferChunk; // 각 쓰레드별(TLS) 사용할 SendBufferChunk