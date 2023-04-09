#pragma once
#include<memory>
namespace BackgroundThread
{
	
	class TaskBase
	{
	public:
		virtual void Run() = 0;
	};
	using ptrTaskBase = std::shared_ptr<TaskBase>;
}