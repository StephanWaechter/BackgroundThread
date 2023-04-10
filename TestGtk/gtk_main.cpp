#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/listbox.h>
#include <gtkmm/progressbar.h>
#include <glibmm/dispatcher.h>
#include <iostream>
#include <BackgroundThread/Thread.hpp>
#include <BackgroundThread/Task.hpp>
#include <future>

using namespace Glib;
using namespace Gtk;
using namespace BackgroundThread;

void DelayedWork(int time_in_seconds, std::function<void(double)> progress, std::function<void(int)> done)
{
	std::cout << "Running " << time_in_seconds << " on thread: " << std::this_thread::get_id() << std::endl;
	int N = time_in_seconds * 10;
	for (int k = 0; k < N; k++)
	{
		progress(k / double(N));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	done(2<<N);
	return;
}

class MyProgress
{
public:
	MyProgress();
	Gtk::Widget & get_widget() { return m_Grid; }
	void set_label(std::string& string);
	void set_task(ptrTaskBase task);
	void set_fraction(double fraction);

private:
	Gtk::Grid m_Grid;
	Gtk::Label m_Label;
	Gtk::ProgressBar m_ProgressBar;
	Gtk::Button m_Button;
	ptrTaskBase m_Task;
};

MyProgress::MyProgress() 
{
	m_Task = nullptr;
	m_Button.set_child(Gtk::Label("X"));
	m_Grid.attach(m_Label, 0, 0);
	m_Grid.attach(m_ProgressBar, 1, 0);
	m_Grid.attach(m_Button, 2, 0);
}

void MyProgress::set_label(std::string& string)
{
	m_Label.set_text(string);
}

void MyProgress::set_task(ptrTaskBase task)
{
	m_Task = task;
}

void MyProgress::set_fraction(double fraction)
{
	m_ProgressBar.set_fraction(fraction);
}

class MyThumb : public Gtk::Button
{
public:
	MyThumb(Glib::ustring const& label);
	void Setup(Glib::ustring const& label);

};

MyThumb::MyThumb(Glib::ustring const& label)
{
	Setup(label);
}

void MyThumb::Setup(Glib::ustring const& label)
{
	set_label(label);
	set_margin(10);
	set_hexpand(true);
	set_vexpand(true);
	set_size_request(-1, 40);
	return;
};

class MyWindow : public Gtk::Window
{
public:
	MyWindow();
	void on_clicked(int time_in_seconds);
	void OnProgress(MyProgress* progress, double fraction);
	void OnWorkDone(MyProgress* progress, Gtk::Widget* row, int result);
	void OnProgressText(double fraction);
	void OnWorkDoneText(int result);

protected:
	std::vector<MyThumb> thumbs;
	Gtk::Grid m_grid;
	Gtk::ListBox m_list;
	MyProgress m_Progress;

	void notify();
	void DoUiWork();
	Glib::Dispatcher m_dispatcher;
	BackgroundThread::Thread m_Threads;
	std::list<MyProgress*> m_ProgressVector;

private:
	void AddButton(int row, int column, std::string const& label, int time);
};

void MyWindow::AddButton(int row, int column, std::string const& label, int time)
{
	thumbs.push_back(MyThumb(label));
	thumbs.back().signal_clicked().connect(
		sigc::bind(sigc::mem_fun(*this, &MyWindow::on_clicked), time)
	);
	m_grid.attach(thumbs.back(), row, column);
}

MyWindow::MyWindow()
{
	set_title("Basic application");
	set_default_size(640, 480);
	
	AddButton(0, 0, "Task 3 seconds", 3);
	AddButton(0, 1, "Task 5 seconds", 5);
	AddButton(0, 2, "Task 10 seconds", 10);

	m_grid.attach(m_list, 7, 0, 1, 6);
	
	set_child(m_grid);

	m_Threads.Start(std::bind(&MyWindow::notify,this));
	m_dispatcher.connect(sigc::mem_fun(*this, &MyWindow::DoUiWork));
}

void MyWindow::notify()
{
	m_dispatcher.emit();
}

void MyWindow::DoUiWork()
{
	m_Threads.DoUiWork();
}

void MyWindow::OnProgress(MyProgress* progress, double fraction)
{
	progress->set_fraction(fraction);
}

void MyWindow::OnWorkDone(MyProgress* progress, Gtk::Widget* row, int result)
{
	progress->set_fraction(1.0);
	m_list.remove(*row);
	delete progress;
}

void MyWindow::OnProgressText(double fraction)
{
	std::cout << fraction << std::endl;
}

void MyWindow::OnWorkDoneText(int result)
{
	std::cout << result << std::endl;
}

void MyWindow::on_clicked(int time_in_seconds)
{
	auto task = BackgroundThread::Task<int>::CreateTask();

	std::string text =std::to_string(time_in_seconds) + " seconds";
	
	MyProgress* progress = new MyProgress();

	progress->set_label(text);
	m_list.append(progress->get_widget());
	auto row = m_list.get_last_child();

	auto fwork = std::bind(&DelayedWork, time_in_seconds, std::placeholders::_1, std::placeholders::_2);
	auto fprogress = std::bind(&MyWindow::OnProgress, this, progress, std::placeholders::_1);
	auto fdone = std::bind(&MyWindow::OnWorkDone, this, progress, row, std::placeholders::_1);
	task->set_Work(fwork);
	task->set_Progress(fprogress);
	task->set_Done(fdone);

	m_Threads.AddTask(
		task
	);
}


int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create("org.gtkmm.examples.base");

	return app->make_window_and_run<MyWindow>(argc, argv);
}
