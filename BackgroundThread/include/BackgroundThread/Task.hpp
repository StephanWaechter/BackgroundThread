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
				{};

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
				forward(
					[this, future]()
					{ 
						m_done(future);
					});
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
		std::function<TResult(std::function<void(double)>, Token* token)> work,
		std::function<void(double)> progress,
		std::function<void(std::shared_future<TResult>)> done,
		std::shared_ptr<Token> token
	)
	{
		return std::make_shared<Task<TResult>>(work, progress, done, token);
	};
}