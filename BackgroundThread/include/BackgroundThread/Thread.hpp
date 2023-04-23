#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include "types.hpp"

namespace BackgroundThread
{
	class Thread
	{

	public:
		void Start(std::function<void(void)> notifier);
		void Stop();
		void Run(t_tasklet task);

		void DoUiWork();
		void ForwardUiWork(t_task& task);
		~Thread();

	private:
		void Process();

		bool m_continue;
		
		std::condition_variable m_cond_var;
		std::mutex m_mutex;
		std::queue<t_tasklet> m_queue;
		
		std::function<void(void)> m_notifier;
		std::mutex m_ui_mutex;
		std::queue<t_task> m_ui_work;

		std::vector<std::thread> m_thread;
	};

}