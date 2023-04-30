#include <iostream>
#include <BackgroundThread/Thread.hpp>
#include <BackgroundThread/Token.hpp>

BackgroundThread::Thread::Thread(std::function<void(void)> notifier, uint32_t num_threads)
{
	m_continue = true;
	m_notifier = notifier;
	m_thread.resize(num_threads);
	for (uint32_t i = 0; i < num_threads; i++) 
	{
		m_thread.at(i) = std::thread( [this] { Process(); } );
	}
}

void BackgroundThread::Thread::Join()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_continue = false;
	}
	m_cond_var.notify_all();
	for (std::thread& active_thread : m_thread) {
		active_thread.join();
	}
}

BackgroundThread::Thread::~Thread()
{
	if(m_continue)
		Join();
}

void BackgroundThread::Thread::Run(std::shared_ptr<BaseTask> task)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push_back( task );
	} // Do not lock m_cond_var.notify_one -> dead lock in Thread::Process
	std::cout << "BackgroundThread::Add task " << " m_queue.size() = " << m_queue.size() << std::endl;
	m_cond_var.notify_one();
}

void BackgroundThread::Thread::DoUiWork()
{
	{
		std::lock_guard<std::mutex> lock(m_ui_mutex);
		while (!m_ui_work.empty())
		{
			auto& work = m_ui_work.front();
			work();
			m_ui_work.pop();
		}
	}
}

void BackgroundThread::Thread::ForwardUiWork(t_task task)
{
	{
		std::lock_guard<std::mutex> lock(m_ui_mutex);
		m_ui_work.push(task);
	} // Do not lock notifier -> dead lock in Thread::DoUiWork
	m_notifier();
}

void BackgroundThread::Thread::Process()
{
	std::cout << "BackgroundThread::Process: Start Thread " << std::this_thread::get_id() << std::endl;
	while (true)
	{
		std::shared_ptr<BaseTask> currentTask;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cond_var.wait(lock, [&] { return !m_continue || !m_queue.empty(); });
			if (!m_continue && m_queue.empty()) 
			{
				std::cout << "BackgroundThread::Process: Exit thread " << std::this_thread::get_id() << std::endl;
				return;
			}
			currentTask = m_queue.front();
			m_queue.pop_front();
		}
		std::cout << "BackgroundThread::Start work on thread: " << std::this_thread::get_id() << " m_queue.size() = " << m_queue.size() << std::endl;
		t_forward_task forward = [=] (t_task task) -> void
		{
			ForwardUiWork(task);
		};
		currentTask->Run(forward);
	}
}