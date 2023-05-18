#include <gtkmm.h>
#include <glibmm.h>
#include <iostream>
#include <Views/MainWindow.hpp>
#include <ViewModels/MainViewModel.hpp>

using namespace GtkTest;

Glib::Dispatcher Dispatcher;
std::unique_ptr<ViewModel::MainViewModel> mainViewModel;

int main(int argc, char* argv[])
{
	auto backgroundThreads = std::make_unique<BackgroundThread::Thread>(
		[d = &Dispatcher] {
			d->emit();
		}, 4
	);

	Dispatcher.connect(
		[t = backgroundThreads.get()]
		{
			t->DoUiWork();
		}
	);

	mainViewModel = std::make_unique<ViewModel::MainViewModel>(
		std::move(backgroundThreads)
	);



	auto app = Gtk::Application::create("org.gtkmm.examples.base");
	return app->make_window_and_run<Views::MainWindow>(argc, argv,*mainViewModel);
}
