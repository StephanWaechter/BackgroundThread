#pragma once
#include "Thumb.hpp"
#include "Progress.hpp"

#include <gtkmm.h>
#include <ThreadPool.hpp>
#include <Task.hpp>
#include <functional>
#include <memory>
#include <chrono>
#include <ViewModels/MainViewModel.hpp>

namespace GtkTest
{
	namespace Views
	{
		class MainWindow : public Gtk::Window
		{
		private:
			ViewModel::MainViewModel& ViewModel;
			std::vector<Thumb> m_thumbs;
			Gtk::Grid m_grid;
			Gtk::ListBox m_list;
			Gtk::Statusbar m_statusbar;

			void AddButton(int row, int column, std::string const& label, int time)
			{
				m_thumbs.push_back(Thumb(label));
				m_thumbs.back().signal_clicked().connect(
					[this,time]() -> void
					{
						ViewModel.StartWork(std::chrono::seconds(time));
					}
				);
				m_grid.attach(m_thumbs.back(), row, column);
			}

			void AddProgress(ViewModel::WorkViewModel& work)
			{
				auto progress = std::make_shared<Progress>(work.get_Name());
				m_list.append(*progress);
				auto row = m_list.get_last_child();

				work.OnProgress = [progress,&work](double value)
				{
					progress->set_fraction(value);
				};

				work.OnDone = [this, row, name = work.get_Name()](int result)
				{
					m_list.remove(*row);
					m_statusbar.push("Work \"" + name + "\" + is done");
				};

				progress->get_AbortButtonClicked().connect(
					[this, &work, row] {
						m_list.remove(*row);
						work.Abort();
					}
				);
			}

		public:
			MainWindow(ViewModel::MainViewModel& mainViewModel) : ViewModel{mainViewModel}
			{
				mainViewModel.OnNewWork = [this](ViewModel::WorkViewModel& work)
				{
					AddProgress(work);
				};

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
			}

			bool on_close_request() override
			{
				ViewModel.OnClosing();
				return false;
			}



		};
	}
}