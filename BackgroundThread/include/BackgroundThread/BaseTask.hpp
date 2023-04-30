#pragma once
#include "types.hpp"

namespace BackgroundThread
{
	class BaseTask
	{
		public:
			BaseTask() {};
			virtual t_task Run() = 0;
	};
}