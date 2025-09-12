#pragma once

#include "NotCopyable.h"

#include <functional>
#include <thread>
#include <queue>
#include <mutex>

class ThreadPool {

public:
	ThreadPool();
	void Start(unsigned int num = std::thread::hardware_concurrency());
	void AddTask(const std::function<void()>& task);
	bool Finished();
	void Stop();


private:
	std::vector<std::thread> Threads;
	std::queue<std::function<void()>> Tasks;

	std::mutex ThreadMutex;
	std::condition_variable MutexCondition;

	bool ShouldStop;
};

