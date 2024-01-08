#pragma once

/*-------------------------------------------
				  GlobalQueue
Job 이 아닌 JobQueue를 받아서 저장하고 있다가 
다른 스레드에서 JobQueue를 Pop하여 사용한다.
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

