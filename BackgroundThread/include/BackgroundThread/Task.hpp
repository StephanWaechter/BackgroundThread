#pragma once
#include "Token.hpp"
#include "types.hpp"

namespace BackgroundThread
{
	template<class TResult> t_tasklet CreateTask(
		std::function<TResult(std::function<void(double)>)> work,
		std::function<void(double)> onProgress,
		std::function<void(TResult)> onDone
	)
	{
		return [=](t_forward_task forward_task)
		{
			auto update_progress = [=](double progress)
			{
				forward_task(
					t_task(
						[=]() { onProgress(progress); }
					)
				);
			};
			TResult result = work(update_progress);
			forward_task([=]() { onDone(result); });
		};
	};

	template<class TResult> t_tasklet CreateTask(
		std::function<TResult(std::function<void(double)>, Token* token)> work,
		std::function<void(double)> onProgress,
		std::function<void(TResult)> onDone,
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
				try
				{
					TResult result = work(update_progress, token.get());
					forward_task([=]() { onDone(result); });
				}
				catch(AbortedException const& e)
				{

				}
			}
		};
	};
}