#pragma once
#include<memory>
namespace BackgroundThread
{
	class Thread;
	class TaskBase
	{
	public:
		virtual void Run(Thread* thread) = 0;
	};
	using ptrTaskBase = std::shared_ptr<TaskBase>;
}