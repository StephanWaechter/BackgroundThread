#pragma once
#include "BackgroundTask.hpp"
#include <thread>
#include <mutex>
#include <queue>

using Notifier = std::function<void()>;
using ptrBackgroundTaskBase = std::shared_ptr<BackgroundTaskBase>;

class BackgroundThread
{
	
public:
	void Start(Notifier notify);
	void Stop();
	void Add(ptrBackgroundTaskBase const& worker);
	void Continue();
	~BackgroundThread();

private:
	void Process();

	Notifier m_notifier;
	std::condition_variable m_cond_var;
	std::mutex m_mutex;
	bool m_continue;
	std::queue<ptrBackgroundTaskBase> m_queue;
	std::queue<ptrBackgroundTaskBase> m_finsihed;
	std::unique_ptr<std::thread> m_thread;
};