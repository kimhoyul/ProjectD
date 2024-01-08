#include "pch.h"
#include "GlobalQueue.h"

/*-------------------------------------------
				  GlobalQueue
Job 이 아닌 JobQueue를 받아서 저장하고 있다가
다른 스레드에서 JobQueue를 Pop하여 사용한다.
-------------------------------------------*/

GlobalQueue::GlobalQueue()
{
}

GlobalQueue::~GlobalQueue()
{
}

void GlobalQueue::Push(JobQueueRef jobQueue)
{
	_jobQueues.Push(jobQueue);
}

JobQueueRef GlobalQueue::Pop()
{
	return _jobQueues.Pop();
}
