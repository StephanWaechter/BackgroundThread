#pragma once
#include "Token.hpp"
#include "types.hpp"

namespace BackgroundThread
{
	template<class TResult> t_tasklet CreateTask(
		std::function<TResult(std::function<void(double)>)> work,
		std::function<void(double)> onProgress,
		std::function<void(std::shared_future<TResult>)> onDone
	)
	{
		return [=](t_forward_task forward_task)
		{
			auto update_progress = [=](double progress)
			{
				forward_task([=]() { onProgress(progress); });
			};
			auto future = std::async(std::launch::deferred, work, update_progress).share();
			future.wait(); // do the work
			forward_task(
				[onDone, future]()
				{
					onDone(future);
				});
		};
	};

	template<class TResult> t_tasklet CreateTask(
		std::function<TResult(std::function<void(double)>, Token* token)> work,
		std::function<void(double)> onProgress,
		std::function<void(std::shared_future<TResult>)> onDone,
		std::shared_ptr<Token> token
	)
	{
		return [=](t_forward_task forward_task)
		{
			auto update_progress = [=](double progress)
			{
				forward_task([=]() { onProgress(progress); });
			};
			
			if(!token->is_Aborted())
			{
				auto future = std::async(std::launch::deferred, work, update_progress, token.get()).share();
				future.wait(); // do the work
				forward_task(
					[onDone, future]()
					{ 
						onDone(future);
					});
			}
		};
	};
}