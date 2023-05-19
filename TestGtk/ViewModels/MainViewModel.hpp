#pragma once
#include <memory>
#include <chrono>
#include <list>
#include <ThreadPool.hpp>
#include "WorkViewModel.hpp"

namespace GtkTest
{
	namespace ViewModel
	{
		class MainViewModel
		{
		public:
			MainViewModel(
				std::unique_ptr<BackgroundThread::ThreadPool> thread
			);

			void StartWork(std::chrono::duration<double> duration);
			std::function<void(WorkViewModel&)> OnNewWork;

			void OnClosing();
		private:
			std::unique_ptr<BackgroundThread::ThreadPool> m_Thread;
			std::list<std::unique_ptr<WorkViewModel>> m_Worker;
		};
	}
}