#pragma once
#include "BaseTask.hpp"
#include "Token.hpp"
#include "types.hpp"

namespace BackgroundThread
{
	template<class TResult> class Task : public BaseTask
	{
		public:
			using f_work = std::function<TResult(f_progress,Token *)>;
			using f_done = std::function<void(std::shared_future<TResult>)>;

			Task(
				f_work work,
				f_progress progress,
				f_done done,
				std::shared_ptr<Token> token
				) :
				BaseTask(token),
				m_work{work},
				m_progress{progress},
				m_done{done}
				{
				int a = 0;
			};

			// Excplicitly delete copy constructor
			Task(Task const& copy) = delete;

			void Run(t_forward_task forward) override
			{
				if(get_Token()->is_Aborted())
				{
					AbortImmediately();
					return;
				}
				auto update_progress = [=](double progress)
				{
					forward([=]() { m_progress(progress); });
				};
				auto future = std::async(std::launch::deferred, m_work, update_progress, get_Token()).share();
				future.wait(); // do the work

				/*
				* store copy of m_done on stack because task might
				* go out of scope before ui work can be completed
				*/ 
				auto onDone = m_done; 
				forward(
					[=]()
					{ 
						onDone(future);
					}
				);
			}

			void AbortImmediately() override
			{
				std::promise<TResult> p;
				p.set_exception(
					std::make_exception_ptr(
						AbortedException("Task aborted before it was started")
					)
				);
				auto future = p.get_future();
				m_done(std::move(future));
			}

		private:
			f_work m_work;
			f_progress m_progress;
			f_done m_done;
	};

	template<class TResult> std::shared_ptr<Task<TResult>> CreateTask(
		std::function<TResult(f_progress notifyProgress, Token* token)> work,
		f_progress onProgress,
		std::function<void(std::shared_future<TResult>)> onDone,
		std::shared_ptr<Token> token
	)
	{
		return std::make_shared<Task<TResult>>(work, onProgress, onDone, token);
	};
}