#pragma once
#include <functional>
namespace BackgroundThread
{
	using action = std::function<void(void)>;
	class AbortedException : public std::runtime_error
	{
	public:
		AbortedException(std::string const& msg) : std::runtime_error(msg) {};
	};
}