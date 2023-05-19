#pragma once
#include <mutex>
#include <queue>
#include <vector>
#include "Types.hpp"
#include "UiWork.hpp"

namespace BackgroundThread
{
	class UiWorker
	{
	public:
		void push(UiWork work);
		void set_Notifer(action notifier);
		void doWork();

		template<class TType>
		std::function<void(TType)> CreateNotifier(
			std::function<void(TType)> function
		)
		{
			return [=](TType argument)
			{
				push(
					UiWork(
						[=]() { function(argument); }
					)
				);
			};
		};

	private:
		action m_notifier;
		std::mutex m_ui_mutex;
		std::queue<UiWork> m_ui_queue;
	};
}