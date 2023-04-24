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
	using t_forward_task = std::function<void(t_task)>;
	using t_tasklet = std::function<void(t_forward_task)>;

	using f_progress = std::function<void(double)>;
}