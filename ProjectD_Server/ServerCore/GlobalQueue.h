#pragma once

/*-------------------------------------------
				  GlobalQueue
Job �� �ƴ� JobQueue�� �޾Ƽ� �����ϰ� �ִٰ� 
�ٸ� �����忡�� JobQueue�� Pop�Ͽ� ����Ѵ�.
-------------------------------------------*/
class GlobalQueue
{
public:
	GlobalQueue();
	~GlobalQueue();

	void Push(JobQueueRef jobQueue);
	JobQueueRef Pop();

private:
	LockQueue<JobQueueRef> _jobQueues;
};

