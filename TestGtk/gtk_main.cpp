#include <gtkmm.h>
#include <glibmm.h>
#include <iostream>
#include <MainWindow.hpp>

int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create("org.gtkmm.examples.base");
	return app->make_window_and_run<BackgroundThread::GtkDemo::MainWindow>(argc, argv);
}
