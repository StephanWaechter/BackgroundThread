#pragma once
#include <functional>
#include <future>

namespace BackgroundThread
{

	class Finalizer {
		virtual void Finalize() = 0;
	};

	class Task : public Finalizer {
		virtual void Run() = 0;
	};

	template<class TInput> class FunctionFinalizer : public Finalizer
	{
	public:
		FunctionFinalizer(std::function<void(TInput)> finally_) : m_finally{ finally_ } {}

		void Finalize() override
		{
			m_finally();
		}

	private:
		std::function<void(TInput)> m_finally;
	};

	template<class TType> class FunctionTask : public Task, public Finalizer
	{
	public:
		FunctionTask(
			std::function<TType(void)> work_,
			std::function<void(std::shared_future<TType>)> finally_) : 
			m_work{ work_ },
			m_finally{ finally_ },
			m_future{ m_promise.get_future().share()},
			m_aborted{false}
		{}

		void Run() override
		{
			std::promise<TResult> promis;
			try
			{
				TResult result = work();
				promis.set_value(result);
			}
			catch (AbortedException)
			{
				m_aborted = true;
			}
			catch (...)
			{
				promis.set_exception(
					std::current_exception()
				);
			}
		}

		void Finalize() override
		{
			if (m_aborted)
				return;
			m_finally(m_future);
		}

	private:
		bool m_aborted;
		std::promise<TType> m_promis;
		std::shared_future<TType> m_future;
		std::function<void(TType)> m_work;
		std::function<void(std::shared_future<TType>)> m_finally;
	};
}