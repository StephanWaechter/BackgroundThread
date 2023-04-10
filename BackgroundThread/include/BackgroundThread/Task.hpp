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
		static std::shared_ptr<Task<TResult>> inline CreateTask()
		{
			return std::make_shared<Task<TResult>>(
				Task<TResult>()
				);
		};

		Task(const Task& o) = default;

		void Run(Thread* thread);

		void set_Work(fwork work);
		void set_Progress(fprogress progress);
		void set_Done(fdone done);

	private:
		fwork m_work;
		fprogress m_onProgress;
		fdone m_onDone;

		Task() :
			m_work{ nullptr },
			m_onProgress{ nullptr },
			m_onDone{ nullptr } {};

		void ForwardProgress(Thread* thread, double progress);
		void ForwardDone(Thread* thread, TResult result);
	};

	template<class TResult>
	inline void BackgroundThread::Task<TResult>::set_Work(Task<TResult>::fwork work)
	{
		m_work = work;
	}

	template<class TResult>
	inline void BackgroundThread::Task<TResult>::set_Progress(Task<TResult>::fprogress progress)
	{
		m_onProgress = progress;
	}

	template<class TResult>
	inline void BackgroundThread::Task<TResult>::set_Done(Task<TResult>::fdone done)
	{
		m_onDone = done;
	}


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