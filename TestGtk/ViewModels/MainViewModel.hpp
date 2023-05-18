#pragma once
#include <memory>
#include <chrono>
#include <list>
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
				std::unique_ptr<BackgroundThread::Thread> thread
			);

			void StartWork(std::chrono::duration<double> duration);
			std::function<void(WorkViewModel&)> OnNewWork;

			void OnClosing();
		private:
			std::unique_ptr<BackgroundThread::Thread> m_Thread;
			std::list<std::unique_ptr<WorkViewModel>> m_Worker;
		};
	}
}