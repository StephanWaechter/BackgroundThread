#pragma once
#include <mutex>
#include "Types.hpp"

namespace BackgroundThread
{
	class Token
	{
	public:
		Token() : m_Aborted{ false } {};

		bool is_Aborted() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_Aborted;
		};

		void ThrowIfAborted() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_Aborted)
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
		mutable std::mutex m_mutex;
		bool m_Aborted;
	};
}