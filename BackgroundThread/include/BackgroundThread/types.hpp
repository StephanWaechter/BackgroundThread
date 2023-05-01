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

	using t_task = std::function<void(void)>;
	template<class T> using action_t = std::function<void(T)>;
	using task_t = std::function<action_t<void>(void)>;
}
