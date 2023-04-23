// CMakeTest.cpp : Defines the entry point for the application.
//

#include <BackgroundThread/Thread.hpp>
#include <BackgroundThread/Task.hpp>
#include <iostream>
#include <string>

using namespace BackgroundThread;

Thread bthread;
void notify()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	bthread.DoUiWork();
}

int work(std::function<void(double)> progress, Token const * const token)
{
	std::cout << "Work: " << std::this_thread::get_id() << std::endl;
	for (int k = 0; k < 10; k++)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Work: " << k << " " << std::this_thread::get_id() << std::endl;
		progress( k / 10.0 );
	}
	
	std::cout << "Done" << std::endl;
	return 5;
}

void progress(double progres)
{
	std::cout << "Progress: " << progress << " " << std::this_thread::get_id() << std::endl;
}

void done(int result)
{
		std::cout << "Result: " << result << std::endl;
}

int main()
{
	auto token = std::shared_ptr<Token>();
	auto task = CreateTask<int>(work, progress, done, token);

	bthread.Start(notify);
	bthread.Run(task);

	std::string line;
	std::getline(std::cin, line);
	bthread.Stop();
	return 0;
}
