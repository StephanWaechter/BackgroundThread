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

void DelayedWork(int x, int y, std::function<void(double)> progress, std::function<void(int)> done)
{
	std::cout << "Running " << x << " * " << y << " on thread: " << std::this_thread::get_id() << std::endl;
	for (int k = 0; k < 20; k++)
	{
		progress(k / 20.0);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	done(x * y);
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
	void on_clicked(int x, int y);
	void OnProgress(MyProgress* progress, double fraction);
	void OnWorkDone(MyProgress* progress, Gtk::Widget* row, int result);
	void OnProgressText(double fraction);
	void OnWorkDoneText(int result);

protected:
	std::vector<MyThumb> thumbs;
	Gtk::Grid m_grid;
	Gtk::Label m_label;
	Gtk::Label m_label_done;
	Gtk::ListBox m_list;
	MyProgress m_Progress;

	void notify();
	void DoUiWork();
	Glib::Dispatcher m_dispatcher;
	BackgroundThread::Thread m_Threads;
	std::list<MyProgress*> m_ProgressVector;
};

MyWindow::MyWindow()
{
	set_title("Basic application");
	set_default_size(640, 480);
	
	for(int x = 1; x <= 5; x++)
	{
		for (int y = 1; y <= 5; y++)
		{	
			int row = y - 1;
			int col = x - 1;
			
			std::string text = "Buton " + std::to_string(x) + "/" + std::to_string(y);
			Glib::ustring data = "Buton " + std::to_string(x) + "/" + std::to_string(y) + " was clicked";

			thumbs.push_back(MyThumb(text));
			thumbs.back().signal_clicked().connect(
				sigc::bind(sigc::mem_fun(*this, &MyWindow::on_clicked), x, y)
			);
			m_grid.attach(thumbs.back(), row, col);
		}
	}
	Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
	css_provider->load_from_data("label {background-color: cyan;}");
	m_label.get_style_context()->add_provider(
		css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	m_label.set_size_request(-1, 30);
	m_grid.attach(m_label, 0, 6, 6);

	css_provider = Gtk::CssProvider::create();
	css_provider->load_from_data("label {background-color: red;}");
	m_label_done.get_style_context()->add_provider(
		css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	m_label_done.set_size_request(-1, 30);
	m_grid.attach(m_label_done, 0, 7, 7);

	
	m_grid.attach(m_list, 7, 0, 1, 6);
	
	set_child(m_grid);

	m_Threads.Start(std::bind(&MyWindow::notify,this));
	m_dispatcher.connect(sigc::mem_fun(*this, &MyWindow::DoUiWork));
}

void MyWindow::notify()
{
	std::cout << "Notify on thread " << std::this_thread::get_id() << std::endl;
	m_dispatcher.emit();
}

void MyWindow::DoUiWork()
{
	std::cout << "DoUiWork on thread " << std::this_thread::get_id() << std::endl;
	m_Threads.DoUiWork();
}

void MyWindow::OnProgress(MyProgress* progress, double fraction)
{
	std::cout << "OnProgress on thread " << std::this_thread::get_id() << std::endl;
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
	m_label_done.set_text(std::to_string(result));
}


void MyWindow::on_clicked(int x, int y)
{
	std::cout << "on_clicked on thread " << std::this_thread::get_id() << std::endl;
	
	auto task = BackgroundThread::Task<int>::CreateTask();

	std::string text = std::to_string(x) + " * " + std::to_string(y);
	
	
	MyProgress* progress = new MyProgress();

	progress->set_label(text);
	m_list.append(progress->get_widget());
	auto row = m_list.get_last_child();

	auto fwork = std::bind(&DelayedWork, x, y, std::placeholders::_1, std::placeholders::_2);
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
