#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <deque>
#include <cstdint>
#include "BaseTask.hpp"

namespace BackgroundThread
{
	class Thread
	{

	public:
		Thread(std::function<void(void)> notifier, uint32_t number_of_threads = std::thread::hardware_concurrency() - 1);
		void Join();
		void Run(std::shared_ptr<BaseTask> task);

		void DoUiWork();
		void ForwardUiWork(t_task& task);
		~Thread();

	private:
		void Process();

		bool m_continue;
		
		std::condition_variable m_cond_var;
		std::mutex m_mutex;
		std::deque<std::shared_ptr<BaseTask>> m_queue;
		
		std::function<void(void)> m_notifier;
		std::mutex m_ui_mutex;
		std::queue<t_task> m_ui_work;

		std::vector<std::thread> m_thread;
	};

}