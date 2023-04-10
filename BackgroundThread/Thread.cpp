#include <iostream>
#include <BackgroundThread/Thread.hpp>
#include <BackgroundThread/Task.hpp>

void BackgroundThread::Thread::Start(Notifier notifier)
{
	m_continue = true;
	m_notifier = notifier;
	if (m_thread == nullptr)
		m_thread = std::make_unique<std::thread>(&BackgroundThread::Thread::Process, this);
}

void BackgroundThread::Thread::Stop()
{
	m_continue = false;
	m_thread->join();
}

BackgroundThread::Thread::~Thread()
{
	if(m_continue)
		Stop();
}

void BackgroundThread::Thread::AddTask(ptrTaskBase task)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push( task );
		std::cout << "BackgroundThread::Add: m_queue.size() " << m_queue.size() << std::endl;
	}
	m_cond_var.notify_one();
}

void BackgroundThread::Thread::DoUiWork()
{
	while (!m_ui_work.empty())
	{
		auto& work = m_ui_work.front();
		work();
		m_ui_work.pop();
	}
}
void BackgroundThread::Thread::ForwardUiWork(std::packaged_task<void()>& task)
{
	m_ui_work.push(std::move(task));
	m_notifier();
}


void BackgroundThread::Thread::Process()
{
	while (m_continue)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cond_var.wait(lock, [&] { return !m_queue.empty(); });
		}
		auto& currentTask = m_queue.front();
		currentTask->Run(this);
		m_finsihed.push(currentTask);
		if(m_notifier != nullptr) m_notifier();
		m_queue.pop();
		std::cout << "BackgroundThread::Process: m_queue.size() " << m_queue.size() << std::endl;
	}
}