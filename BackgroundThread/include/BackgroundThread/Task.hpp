#pragma once
#include <thread>
#include <mutex>
#include <functional>
#include "TaskBase.hpp"
#include "Thread.hpp"

namespace BackgroundThread
{
	template<class TResult>
	class Task : public TaskBase
	{
	using fprogress = std::function<void(double)>;
	using fdone = std::function<void(TResult)>;
	using fwork = std::function<void(fprogress, fdone)>;
	public:
		static ptrTaskBase inline CreateTask(
			fwork work,
			fprogress onProgress,
			fdone onDone) 
		{
			return std::make_shared<Task<TResult>>(
				Task<TResult>(
					work,
					onProgress,
					onDone
					)
				);
		};

		Task(const Task& o) = default;

		void Run(Thread* thread);

	private:
		fwork m_work;
		fprogress m_onProgress;
		fdone m_onDone;

		Task(
			fwork work,
			fprogress onProgress,
			fdone onDone) :
			m_work{ work },
			m_onProgress{ onProgress },
			m_onDone{ onDone } {};

		void ForwardProgress(Thread* thread, double progress);
		void ForwardDone(Thread* thread, TResult result);
	};

	template<class TResult>
	inline void Task<TResult>::Run(Thread* thread)
	{
		m_work(
			std::bind(&Task<TResult>::ForwardProgress, this, thread, std::placeholders::_1),
			std::bind(&Task<TResult>::ForwardDone, this, thread, std::placeholders::_1)
		);
	}

	template<class TResult>
	inline void Task<TResult>::ForwardProgress(Thread* thread, double progress)
	{
		thread->ForwardUiWork(std::packaged_task<void()>(std::bind(m_onProgress, progress)));
	}

	template<class TResult>
	inline void Task<TResult>::ForwardDone(Thread* thread, TResult result)
	{
		thread->ForwardUiWork(std::packaged_task<void()>(std::bind(m_onDone, result)));
	}

}