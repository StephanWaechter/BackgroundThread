#pragma once
#include "Task.hpp"
#include <thread>
#include <mutex>
#include <queue>

namespace BackgroundThread
{

using Notifier = std::function<void()>;
using ptrTaskBase = std::shared_ptr<TaskBase>;

class Thread
{
	
public:
	void Start(Notifier notify);
	void Stop();
	void Add(ptrTaskBase const& worker);
	void Continue();
	~Thread();

private:
	void Process();

	Notifier m_notifier;
	std::condition_variable m_cond_var;
	std::mutex m_mutex;
	bool m_continue;
	std::queue<ptrTaskBase> m_queue;
	std::queue<ptrTaskBase> m_finsihed;
	std::unique_ptr<std::thread> m_thread;
};

}