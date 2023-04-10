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
			Thread& thread,
			fwork work,
			fprogress onProgress,
			fdone onDone) 
		{
			return std::make_shared<Task<TResult>>(
				Task<TResult>(
					thread,
					work,
					onProgress,
					onDone
					)
				);
		};


		
		Task(const Task& o) = default;

		void Run();

	private:
		Thread& m_thread;
		fwork m_work;
		fprogress m_onProgress;
		fdone m_onDone;

		Task(
			Thread& thread,
			fwork work,
			fprogress onProgress,
			fdone onDone) :
			m_thread{ thread },
			m_work{ work },
			m_onProgress{ onProgress },
			m_onDone{ onDone } {};

		void ForwardProgress(double progress);
		void ForwardDone(TResult result);
	};

	template<class TResult>
	inline void Task<TResult>::Run()
	{
		m_work(
			std::bind(&Task<TResult>::ForwardProgress, this, std::placeholders::_1),
			std::bind(&Task<TResult>::ForwardDone, this, std::placeholders::_1)
		);
	}

	template<class TResult>
	inline void Task<TResult>::ForwardProgress(double progress)
	{
		m_thread.ForwardUiWork(std::packaged_task<void()>(std::bind(m_onProgress, progress)));
	}

	template<class TResult>
	inline void Task<TResult>::ForwardDone(TResult result)
	{
		m_thread.ForwardUiWork(std::packaged_task<void()>(std::bind(m_onDone, result)));
	}

}