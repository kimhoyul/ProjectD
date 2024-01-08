#include "pch.h"
#include "GlobalQueue.h"

/*-------------------------------------------
				  GlobalQueue
Job �� �ƴ� JobQueue�� �޾Ƽ� �����ϰ� �ִٰ�
�ٸ� �����忡�� JobQueue�� Pop�Ͽ� ����Ѵ�.
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
