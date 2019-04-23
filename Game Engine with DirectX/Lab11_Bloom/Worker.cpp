#include "stdafx.h"
#include "Worker.h"
#include "JobManager.h"
#include "Job.h"


Worker::Worker()
{
}


Worker::~Worker()
{
}

void Worker::Begin()
{
	mThread = new std::thread(Loop);
}

void Worker::End()
{
	mThread->join();
	delete mThread;
}

void Worker::Loop()
{
	while (true) {
		if (JobManager::mShutdown) {
			return;
		}
		Job* curr = nullptr;
		JobManager::mLock.lock();
		if (!JobManager::mJobs.empty()) {
			curr = JobManager::mJobs.front();
			JobManager::mJobs.pop();
		}
		JobManager::mLock.unlock();
		if (curr != nullptr) {
			curr->DoIt();
			JobManager::mCounter--;
		}
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		Sleep(1);
	}
}
