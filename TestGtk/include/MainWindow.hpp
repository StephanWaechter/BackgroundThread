#pragma once
#include "Thumb.hpp"
#include "Progress.hpp"

#include <gtkmm.h>
#include <BackgroundThread/Thread.hpp>
#include <BackgroundThread/Task.hpp>
#include <functional>
#include <memory>
#include <chrono>

namespace BackgroundThread
{
	namespace GtkDemo
	{
		struct Work
		{
			Work(std::unique_ptr<Progress> pprogress, std::unique_ptr<Token> ptoken) : pProgress{ std::move(pprogress) }, pToken{ std::move(ptoken) } {};
			std::unique_ptr<Progress> pProgress;
			std::unique_ptr<Token> pToken;
		};

		int DelayedWork(int time_in_seconds, std::function<void(double)> progress, BackgroundThread::Token & token)
		{
			std::cout << "Running " << time_in_seconds << " on thread: " << std::this_thread::get_id() << std::endl;
			int N = time_in_seconds * 10;
			for (int k = 0; k < N; k++)
			{
				progress(k / double(N));
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				if (token.is_Aborted())
					std::cout << "Thread " << std::this_thread::get_id() << " was aborted!!!" << std::endl;
				token.ThrowIfAborted();
			}
			std::cout << "Thread " << std::this_thread::get_id() << " finished." << std::endl;
			return (time_in_seconds);
		}

		class MainWindow : public Gtk::Window
		{
		private:
			std::vector<Thumb> m_thumbs;
			Gtk::Grid m_grid;
			Gtk::ListBox m_list;
			Gtk::Statusbar m_statusbar;

			Glib::Dispatcher m_dispatcher;
			std::unique_ptr<BackgroundThread::Thread> m_Threads;
			std::list<std::shared_ptr<Work>> m_Worker;

			void StartWork(int time_in_seconds)
			{
				std::string text = std::to_string(time_in_seconds) + " seconds";

				m_Worker.push_back(
					std::make_shared<Work>(
						std::make_unique<Progress>(text),
						std::make_unique<Token>()
					)
				);
				auto work = m_Worker.back();
				auto& pProgress = work->pProgress;
				auto& pToken = work->pToken;

				m_list.append(*pProgress);
				auto row = m_list.get_last_child();

				pProgress->get_AbortButtonClicked().connect(
					[=, pToken = pToken.get() ]() -> void
					{
						pToken->Abort();
						m_list.remove(*row);
						m_Worker.remove(work);
					}
				);

				auto notifyProgress = m_Threads->CreateNotifier<double>(
					[=, ptoken = pToken.get(), pprogress = pProgress.get()](double vprogress) -> void
					{
						if (!ptoken->is_Aborted())
						{
							pprogress->set_fraction(vprogress);
						}
					}
				);

				auto task = BackgroundThread::CreateTask<int>(
					[=, ptoken = work->pToken.get()]() -> int
					{
						ptoken->ThrowIfAborted();
						return DelayedWork(time_in_seconds, notifyProgress, *ptoken);
					},
					[=](std::shared_future<int> result)
					{
						try
						{
							auto str = std::to_string(result.get());
							std::string message = "Task with " + str + "s endend";
							m_statusbar.push(message, 0);
						}
						catch (std::exception const& e)
						{
							m_statusbar.push(e.what(), 0);
						}
						catch (...)
						{
							m_statusbar.push("Unknown Exception", 0);
						}
						m_list.remove(*row);
						m_Worker.remove(work);
					}
				);

				m_Threads->Run(task);
			}

			void AddButton(int row, int column, std::string const& label, int time)
			{
				m_thumbs.push_back(Thumb(label));
				m_thumbs.back().signal_clicked().connect(
					[=]() -> void
					{
						StartWork(time);
					}
				);
				m_grid.attach(m_thumbs.back(), row, column);
			}

		public:
			MainWindow()
			{
				set_title("Background-Thread GtkDemo");
				set_default_size(640, 480);

				AddButton(0, 0, "Task 1 seconds", 1);
				AddButton(0, 1, "Task 3 seconds", 3);
				AddButton(0, 2, "Task 5 seconds", 5);
				AddButton(0, 3, "Task 10 seconds", 10);

				m_list.set_selection_mode(Gtk::SelectionMode::NONE);
				m_grid.attach(m_list, 1, 0, 1, 4);

				m_grid.attach(m_statusbar, 0, 4, 2, 1);

				set_child(m_grid);

				m_Threads = std::make_unique<BackgroundThread::Thread>(
					[this]
					{
						m_dispatcher.emit();
					},
					4
				);
				m_dispatcher.connect(
					[=] {
						m_Threads->DoUiWork();
					}
				);
			}

			bool on_close_request() override
			{
				for (auto const& worker : m_Worker)
				{
					worker->pToken->Abort();
				}

				return false;
			}



		};
	}
}