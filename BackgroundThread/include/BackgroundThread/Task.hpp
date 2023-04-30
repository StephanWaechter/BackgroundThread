#pragma once
#include "BaseTask.hpp"
#include "Token.hpp"
#include "types.hpp"

namespace BackgroundThread
{
	template<class TResult> class Task : public BaseTask
	{
		public:
			using f_work = std::function<TResult()>;
			using f_done = std::function<void(std::shared_future<TResult>)>;

			Task(
				f_work work,
				f_done done
				) :
				m_work{work},
				m_done{done}
				{};

			// Excplicitly delete copy constructor
			Task(Task const& copy) = delete;

			t_task Run() override
			{
				std::promise<TResult> promis;
				bool aborted = false;
				try
				{
					TResult result = m_work();
					promis.set_value(result);
				}
				catch (AbortedException)
				{
					aborted = true;
				}
				catch (...)
				{
					promis.set_exception(
						std::current_exception()
					);
				}

				// don't call onDone if we where aborted
				if (aborted) return nullptr;

				auto future = promis.get_future().share();
				/*
				* store copy of m_done on stack because task might
				* go out of scope before ui work can be completed
				*/
				auto onDone = m_done;
				return [=] { onDone(future); };
			}

		private:
			f_work m_work;
			f_done m_done;
	};

	template<class TResult> std::shared_ptr<Task<TResult>> CreateTask(
		std::function<TResult(void)> work,
		std::function<void(std::shared_future<TResult>)> onDone
	)
	{
		return std::make_shared<Task<TResult>>(work, onDone);
	};
}