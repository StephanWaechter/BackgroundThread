#pragma once
#include <memory>
#include <chrono>
#include <vector>
#include <BackgroundThread/Thread.hpp>
#include "WorkViewModel.hpp"

namespace GtkTest
{
	namespace ViewModel
	{
		class MainViewModel
		{
		public:
			MainViewModel(
				BackgroundThread::Thread& thread
			);

			void StartWork(std::chrono::duration<double> duration);
			std::function<void(WorkViewModel&)> OnNewWork;

			void OnClosing();
		private:
			BackgroundThread::Thread& m_Thread;
			std::vector<std::unique_ptr<WorkViewModel>> m_Worker;
		};
	}
}