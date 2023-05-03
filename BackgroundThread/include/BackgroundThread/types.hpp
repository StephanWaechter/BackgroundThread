#pragma once
#include <functional>
#include <future>

namespace BackgroundThread
{
	class AbortedException : public std::runtime_error
	{
	public:
		AbortedException(std::string const& msg) : std::runtime_error(msg) {};
	};

	using action_t = std::function<void(void)>;
	using task_t = std::function<std::function<void(void)>(void)>;
}
