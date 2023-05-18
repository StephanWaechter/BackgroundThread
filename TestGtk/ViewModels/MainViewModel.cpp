#include "MainViewModel.hpp"
#include <BackgroundThread/Task.hpp>
#include <string>
using namespace std::chrono;

GtkTest::ViewModel::MainViewModel::MainViewModel(BackgroundThread::Thread& thread) :
	m_Thread{thread}
{
}

void GtkTest::ViewModel::MainViewModel::StartWork(std::chrono::duration<double> duration)
{
	auto name = std::to_string(duration_cast<seconds>(duration).count()) + " seconds";
	m_Worker.push_back(
		std::move(
			std::make_unique<WorkViewModel>(
				name,
				duration,
				m_Thread
				)
		)
	);
	OnNewWork(*(m_Worker.back()));
	auto task = BackgroundThread::CreateTask<int>(
		[worker = m_Worker.back().get()]() {
			return worker->Work();
		},
		[worker = m_Worker.back().get()](std::shared_future<int> result)
		{
			worker->OnDone(result.get());
		}
		);
	m_Thread.Run(task);
}

void GtkTest::ViewModel::MainViewModel::OnClosing()
{
}
