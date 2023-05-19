#pragma once
#include <memory>
#include <chrono>
#include <Token.hpp>
#include <ThreadPool.hpp>
namespace GtkTest
{
	namespace ViewModel
	{
		class WorkViewModel
		{
		public:
			WorkViewModel(
				std::string const& name,
				std::chrono::duration<double> duration,
				BackgroundThread::ThreadPool& thread
			);
			void Abort();

			std::function<int(void)> Work;
			std::function<void(double)> OnProgress;
			std::function<void(int)> OnDone;
			inline std::string const& get_Name() { return m_Name; }

		private:
			std::string m_Name;
			std::chrono::milliseconds m_Duration;
			BackgroundThread::Token m_Token;
			
		};
	}
}