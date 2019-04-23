#include "stdafx.h"
#include "JobManager.h"
#include "Worker.h"

volatile bool JobManager::mShutdown = false;
std::mutex JobManager::mLock;
std::queue<Job*> JobManager::mJobs;
std::atomic<int> JobManager::mCounter = 0;

JobManager::JobManager()
{
	mWorker1 = new Worker();
	mWorker2 = new Worker();
	mWorker3 = new Worker();
	mWorker4 = new Worker();
}


JobManager::~JobManager()
{
	delete mWorker1;
	delete mWorker2;
	delete mWorker3;
	delete mWorker4;
}

void JobManager::Begin()
{
	if (mWorker1 != nullptr)
	mWorker1->Begin();
	if (mWorker2 != nullptr)
	mWorker2->Begin();
	if (mWorker3 != nullptr)
	mWorker3->Begin();
	if (mWorker4 != nullptr)
	mWorker4->Begin();
}

void JobManager::End()
{
	mShutdown = true;
	int size = JobManager::mJobs.size();
	if (mWorker1 != nullptr)
		mWorker1->End();
	if (mWorker2 != nullptr)
		mWorker2->End();
	if (mWorker3 != nullptr)
		mWorker3->End();
	if (mWorker4 != nullptr)
		mWorker4->End();
}

void JobManager::AddJob(Job * job)
{
	JobManager::mLock.lock();
	mJobs.push(job);
	JobManager::mLock.unlock();
	mCounter++;
}

void JobManager::WaitForJobs()
{
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	Sleep(1);
}
