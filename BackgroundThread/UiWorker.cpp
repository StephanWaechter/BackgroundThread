#include "UiWorker.hpp"

void BackgroundThread::UiWorker::push(UiWork work)
{
	{
		std::lock_guard<std::mutex> lock(m_ui_mutex);
		m_ui_queue.push(std::move(work));
	} // Do not lock notifier -> dead lock in Thread::DoUiWork
	m_notifier();
}

void BackgroundThread::UiWorker::set_Notifer(action notifier)
{
	m_notifier = notifier;
}

void BackgroundThread::UiWorker::doWork()
{
	std::lock_guard<std::mutex> lock(m_ui_mutex);
	while (!m_ui_queue.empty())
	{
		auto& work = m_ui_queue.front();
		work.Exceute();
		m_ui_queue.pop();
	}
}
