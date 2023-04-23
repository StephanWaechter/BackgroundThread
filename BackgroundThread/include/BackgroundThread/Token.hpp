#pragma once
#include <mutex>
#include "types.hpp"

namespace BackgroundThread
{
	class Token
	{
	public:
		Token() : m_Aborted{ false } {};

		bool is_Aborted()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_Aborted;
		};

		void ThrowIfAborted()
		{
			if(m_Aborted)
			{
				throw AbortedException("Thread was Aborted");
			}
		}

		void Abort()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_Aborted = true;
		};

	private:
		std::mutex m_mutex;
		bool m_Aborted;

	};
}