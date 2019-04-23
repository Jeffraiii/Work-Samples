#pragma once
#include <queue>
#include <mutex>
#include <atomic>

class Worker;
class Job;
class JobManager
{
public:
	JobManager();
	~JobManager();
	static volatile bool mShutdown;
	void Begin();
	void End();
	static void AddJob(Job* job);
	void WaitForJobs();
	static std::queue<Job*> mJobs;
	static std::mutex mLock;
	static std::atomic<int> mCounter;

private:
	Worker* mWorker1;	
	Worker* mWorker2;
	Worker* mWorker3;
	Worker* mWorker4;
};

