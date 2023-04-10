#pragma once
#include "TaskBase.hpp"
#include <thread>
#include <mutex>
#include <queue>
#include <future>

namespace BackgroundThread
{

	using Notifier = std::function<void()>;

	class Thread
	{

	public:
		void Start(Notifier notify);
		void Stop();
		void AddTask(
			ptrTaskBase task
		);

		void DoUiWork();
		void ForwardUiWork(std::packaged_task<void()>& task);
		~Thread();

	private:
		void Process();

		bool m_continue;
		Notifier m_notifier;
		std::mutex m_mutex;
		std::mutex m_ui_mutex;
		std::condition_variable m_cond_var;
		std::queue<ptrTaskBase> m_queue;
		std::vector<std::thread> m_thread;
		std::queue<std::packaged_task<void()>> m_ui_work;
	};

}