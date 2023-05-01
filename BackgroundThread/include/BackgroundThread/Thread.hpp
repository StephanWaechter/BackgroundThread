#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <deque>
#include <cstdint>
#include "types.hpp"

namespace BackgroundThread
{
	class Thread
	{

	public:
		Thread(std::function<void(void)> notifier, uint32_t number_of_threads = std::thread::hardware_concurrency() - 1);
		void Join();
		void Run(task_t task);

		void DoUiWork();

		void ForwardUiWork(t_task task);

		template<class TType>
		std::function<void(TType)> CreateNotifier(std::function<void(TType)> onEvent)
		{
			return [=](TType argument)
			{
				ForwardUiWork([=]() { onEvent(argument); });
			};
		};

		~Thread();

	private:
		void Process();

		bool m_continue;
		
		std::condition_variable m_cond_var;
		std::mutex m_mutex;
		std::deque<task_t> m_queue;
		
		action_t<void> m_notifier;
		std::mutex m_ui_mutex;
		std::queue<action_t<void>> m_ui_work;

		std::vector<std::thread> m_thread;
	};

}