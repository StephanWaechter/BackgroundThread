#pragma once
#include "Token.hpp"
#include "types.hpp"

namespace BackgroundThread
{
	template<class TResult> task_t CreateTask(
		std::function<TResult(void)> work
	)
	{
		return [=]() -> action_t<void>
		{
			std::promise<TResult> promis;
			bool aborted = false;
			try
			{
				TResult result = work();
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

			return action_t<void>{nullptr};
		};
	}

	template<class TResult> task_t CreateTask(
		std::function<TResult(void)> work,
		std::function<void(std::shared_future<TResult>)> done
	)
	{
		return [=]() -> action_t<void>
		{
			std::promise<TResult> promis;
			bool aborted = false;
			try
			{
				TResult result = work();
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
			if (aborted) return action_t<void>{nullptr};

			auto future = promis.get_future().share();
			return [=] { done(future); };
		};
	}
}