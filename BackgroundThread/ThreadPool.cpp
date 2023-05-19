#include "ThreadPool.hpp"
#include <iostream>

BackgroundThread::ThreadPool::ThreadPool(int n) : m_continue{true}
{
	for (int k = 0; k < n; k++)
	{
		m_Threads.push_back(std::thread([this] { Process(); }));
	}
}

BackgroundThread::ThreadPool::~ThreadPool()
{
	if (m_continue)
		join();
}

void BackgroundThread::ThreadPool::push(Task task)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_work_queue.push(std::move(task));
	} // Do not lock m_cond_var.notify_one -> dead lock in Thread::Process
	std::cout << "BackgroundThread::Add task " << " m_work_queue.size() = " << m_work_queue.size() << std::endl;
	m_condition_variable.notify_one();
}

void BackgroundThread::ThreadPool::join()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_continue = false;
	}
	m_condition_variable.notify_all();
	for (std::thread& active_thread : m_Threads) {
		active_thread.join();
	}
}

void BackgroundThread::ThreadPool::Process()
{
	std::cout << "BackgroundThread::Process: Start Thread " << std::this_thread::get_id() << std::endl;
	while (true)
	{
		Task currentTask;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_condition_variable.wait(lock,
				[&] { 
					return !m_continue || !m_work_queue.empty(); }
			);
			if (!m_continue && m_work_queue.empty())
			{
				std::cout << "BackgroundThread::Process: Exit thread " << std::this_thread::get_id() << std::endl;
				return;
			}
			currentTask = std::move( m_work_queue.front() );
			m_work_queue.pop();
		}

		std::cout << "BackgroundThread::Start work on thread: " << std::this_thread::get_id() << " m_work_queue.size() = " << m_work_queue.size() << std::endl;
		currentTask.Execute();
		if( currentTask.Completed() )
		{
			m_uiworker.push(
				currentTask.GetUiWork()
			);
		}
	}
}
