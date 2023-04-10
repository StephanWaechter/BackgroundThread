#include <iostream>
#include <BackgroundThread/Thread.hpp>
#include <BackgroundThread/Task.hpp>

void BackgroundThread::Thread::Start(Notifier notifier)
{
	m_continue = true;
	m_notifier = notifier;
	const uint32_t num_threads = 4;
	m_thread.resize(num_threads);
	for (uint32_t i = 0; i < num_threads; i++) 
	{
		m_thread.at(i) = std::thread(std::bind( & Thread::Process, this));
	}
}

void BackgroundThread::Thread::Stop()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_continue = false;
	}
	m_cond_var.notify_all();
	for (std::thread& active_thread : m_thread) {
		active_thread.join();
	}
	m_thread.clear();
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
	}
	std::cout << "BackgroundThread::Add task " << " m_queue.size() = " << m_queue.size() << std::endl;
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
	std::lock_guard<std::mutex> lock(m_ui_mutex);
	m_ui_work.push(std::move(task));
	m_notifier();
}

void BackgroundThread::Thread::Process()
{
	std::cout << "BackgroundThread::Process: Start Thread " << std::this_thread::get_id() << std::endl;
	while (true)
	{
		ptrTaskBase currentTask;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cond_var.wait(lock, [&] { return !m_continue || !m_queue.empty(); });
			if (!m_continue) {
				std::cout << "BackgroundThread::Process: Exit thread " << std::this_thread::get_id() << std::endl;
				return;
			}
			currentTask = m_queue.front();
			m_queue.pop();
		}
		std::cout << "BackgroundThread::Start work on thread: " << std::this_thread::get_id() << " m_queue.size() = " << m_queue.size() << std::endl;
		currentTask->Run(this);
	}
}