#pragma once
#include <thread>
#include <deque>
#include <mutex>

class Job;

class Worker
{
public:
	Worker();
	~Worker();
	void Begin();
	void End();
	static void Loop();
private:
	std::thread* mThread;
};

