#include "MainViewModel.hpp"
#include <Task.hpp>
#include <string>
using namespace std::chrono;

GtkTest::ViewModel::MainViewModel::MainViewModel(std::unique_ptr<BackgroundThread::ThreadPool> thread) :
	m_Thread{std::move(thread)}
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
				*m_Thread
				)
		)
	);
	OnNewWork(*(m_Worker.back()));
	auto task = BackgroundThread::Task::Create<int>(
		[worker = m_Worker.back().get()]() {
			return worker->Work();
		},
		[this, &worker = m_Worker.back()](std::shared_future<int> result)
		{
			worker->OnDone(result.get());
			m_Worker.remove(worker);
		}
		);
	m_Thread->push(std::move(task));
}

void GtkTest::ViewModel::MainViewModel::OnClosing()
{
	for (auto const& worker : m_Worker)
	{
		worker->Abort();
	}
	m_Thread->join();
}
