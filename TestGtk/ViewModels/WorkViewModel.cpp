#include "WorkViewModel.hpp"
#include <iostream>


using namespace std::chrono;

GtkTest::ViewModel::WorkViewModel::WorkViewModel(
	std::string const& name,
	std::chrono::duration<double> duration,
	BackgroundThread::Thread& thread
)
	:
	m_Name {name},
	m_Duration{ duration_cast<milliseconds>(duration) }
{
	auto progress = thread.CreateNotifier<double>(
		[this](double value) {
			if (m_Token.is_Aborted()) return;
			OnProgress(value);
		}
	);

	Work = [this, progress]
	{
		auto start = system_clock::now();
		::duration<double> runtime{ 0ms };
		std::cout << "Running " << m_Duration.count() << " on thread: " << std::this_thread::get_id() << std::endl;
		do
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (m_Token.is_Aborted())
				std::cout << "Thread " << std::this_thread::get_id() << " was aborted!!!" << std::endl;
			m_Token.ThrowIfAborted();

			runtime = system_clock::now() - start;
			progress(runtime / m_Duration);
		} while (runtime <= m_Duration);
		std::cout << "Thread " << std::this_thread::get_id() << " finished." << std::endl;
		return (m_Duration.count());
	};
}

void GtkTest::ViewModel::WorkViewModel::Abort()
{
	m_Token.Abort();
}
