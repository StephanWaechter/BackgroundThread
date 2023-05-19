#pragma once
#include <thread>
#include <queue>
#include <vector>
#include <future>
#include "Types.hpp"
#include "Task.hpp"
#include "UiWorker.hpp"
namespace BackgroundThread
{
	class ThreadPool
	{
	public:
		ThreadPool(int n);
		~ThreadPool();
		void push(Task task);
		void join();

		inline UiWorker& get_UiWorker() { return m_uiworker; }
		
	private:
		void Process();
		bool m_continue;
		action m_notifier;
		std::mutex m_mutex;
		std::condition_variable m_condition_variable;
		std::queue<Task> m_work_queue;
		std::vector<std::thread> m_Threads;
		UiWorker m_uiworker;

	};

}