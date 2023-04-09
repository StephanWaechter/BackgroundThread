#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/listbox.h>
#include <glibmm/dispatcher.h>
#include <iostream>
#include <BackgroundThread/BackgroundThread.hpp>
#include <future>

using namespace Glib;
using namespace Gtk;

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
	void OnWorkDone(int x, int y, int result);

protected:
	std::vector<MyThumb> thumbs;
	Gtk::Grid m_grid;
	Gtk::Label m_label;
	Gtk::Label m_label_done;
	Gtk::ListBox m_list;

	void notify();
	Glib::Dispatcher m_dispatcher;
	BackgroundThread m_Threads;
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
	m_dispatcher.connect(sigc::mem_fun(m_Threads, &BackgroundThread::Continue));
}

void MyWindow::notify()
{
	m_dispatcher.emit();
}

int DelayedWork(int x, int y)
{
	std::cout << "Running " << x << " * " << y << " on thread: " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(3));
	return x * y;
}

void MyWindow::OnWorkDone(int x, int y, int result)
{
	std::stringstream meassage;
	meassage << "Result for " << x << " * " << y << " = " << result << " on thread: " << std::this_thread::get_id() << std::endl;
	m_list.remove(*m_list.get_first_child());
	m_label_done.set_text(meassage.str());
	std::cout << meassage.str();
}

void MyWindow::on_clicked(int x, int y)
{
	std::stringstream message;
	message << "Clicked " << x << " * " << y << " on thread " << std::this_thread::get_id() << std::endl;
	std::cout << message.str();
	
	std::stringstream listitemtext;
	listitemtext << "Running: " << x << " * " << y << std::endl;
	m_list.append(Gtk::Label(listitemtext.str()));

	m_label.set_text(message.str());
	m_Threads.Add(
		std::make_shared<BackgroundTask<int>>(
			BackgroundTask<int>(
				std::bind(&DelayedWork, x, y),
				std::bind(&MyWindow::OnWorkDone, this, x, y, std::placeholders::_1)
				)
			)
	);
}



int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create("org.gtkmm.examples.base");

	return app->make_window_and_run<MyWindow>(argc, argv);
}