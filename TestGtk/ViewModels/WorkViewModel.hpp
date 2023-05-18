#pragma once
#include <memory>
#include <chrono>
#include <BackgroundThread/Token.hpp>
#include <BackgroundThread/Thread.hpp>
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
				BackgroundThread::Thread& thread
			);
			void Abort();

			std::function<int(void)> Work;
			std::function<void(double)> OnProgress;
			std::function<void(int)> OnDone;
			std::function<void(void)> OnAbort;
			inline std::string const& get_Name() { return m_Name; }

		private:
			std::string m_Name;
			std::chrono::milliseconds m_Duration;
			double m_Progress;
			BackgroundThread::Token m_Token;
			
		};
	}
}