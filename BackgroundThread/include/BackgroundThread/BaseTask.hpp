#pragma once
#include "types.hpp"

namespace BackgroundThread
{
	class BaseTask
	{
		public:
			BaseTask() {};
			virtual void Run(t_forward_task forward) = 0;
	};
}