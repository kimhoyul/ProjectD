#include "pch.h"
#include "CoreTLS.h"

thread_local uint32				LThreadId = 0; // ���������� ������ ������ ID
thread_local uint64				LEndTickCount = 0; 
thread_local std::stack<int32>	LLockStack;	// �� �����庰(TLS) ����� �� ����
thread_local SendBufferChunkRef	LSendBufferChunk; // �� �����庰(TLS) ����� SendBufferChunk
thread_local JobQueue*			LCurrentJobQueue = nullptr; // �����尡 � ���� ó���ϰ� �ִ��� Ȯ���ϴ� �뵵