#include <BackgroundThread/BackgroundThread.hpp>
#include <iostream>

void BackgroundThread::Start(Notifier notifier)
{
	m_continue = true;
	m_notifier = notifier;
	if (m_thread == nullptr)
		m_thread = std::make_unique<std::thread>(&BackgroundThread::Process, this);
}

void BackgroundThread::Stop()
{
	m_continue = false;
	m_thread->join();
}

BackgroundThread::~BackgroundThread()
{
	if(m_continue)
		Stop();
}

void BackgroundThread::Add(ptrBackgroundTaskBase const& worker)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(worker);
		std::cout << "BackgroundThread::Add: m_queue.size() " << m_queue.size() << std::endl;
	}
	m_cond_var.notify_one();
}

void BackgroundThread::Continue()
{
	while (!m_finsihed.empty())
	{
		auto& task = m_finsihed.front();
		task->Finalize();
		m_finsihed.pop();
	}
} 

void BackgroundThread::Process()
{
	while (m_continue)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cond_var.wait(lock, [&] { return !m_queue.empty(); });
		}
		auto& currentTask = m_queue.front();
		currentTask->Run();
		m_finsihed.push(currentTask);
		if(m_notifier != nullptr) m_notifier();
		m_queue.pop();
		std::cout << "BackgroundThread::Process: m_queue.size() " << m_queue.size() << std::endl;
	}
}