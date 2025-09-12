#include "ThreadPool.h"

ThreadPool::ThreadPool() : ShouldStop(false) {

}

void ThreadPool::Start(unsigned int num) {

	ShouldStop = false;

	for (unsigned int i = 0; i < num; i++)

		Threads.emplace_back([this]() {

			while (true) {
				std::function<void()> t;
				{
					std::unique_lock<std::mutex> lock(ThreadMutex);
					MutexCondition.wait(lock, [this]() { // wait until there is a task to do or until we should stop
						return !Tasks.empty() || ShouldStop;
					});

					if (Tasks.empty() && ShouldStop) // if we have no more tasks and we should stop then end
						return;

					t = Tasks.front();
					Tasks.pop();
				}
				t();
			}

		});
}

void ThreadPool::AddTask(const std::function<void()>& task) {
	{
		std::unique_lock<std::mutex> lock(ThreadMutex);
		Tasks.emplace(task);
	}
	MutexCondition.notify_one();
}

bool ThreadPool::Finished() {
	bool finished = false;
	{
		std::unique_lock<std::mutex> lock(ThreadMutex);
		finished = Tasks.empty();
	}
	return finished;
}

void ThreadPool::Stop() {
	{
		std::unique_lock<std::mutex> lock(ThreadMutex);
		ShouldStop = true;
	}

	MutexCondition.notify_all();

	for (std::thread& t : Threads)
		t.join();

	Threads.clear();
}