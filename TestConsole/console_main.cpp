// CMakeTest.cpp : Defines the entry point for the application.
//

#include <BackgroundThread/Thread.hpp>
#include <BackgroundThread/Task.hpp>
#include <iostream>
#include <string>
using namespace std;
using namespace BackgroundThread;

Thread bthread;
void notify()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	bthread.DoUiWork();
}

void work(std::function<void(double)> progress, std::function<void(int)> done)
{
	std::cout << "Work: " << std::this_thread::get_id() << std::endl;
	for (int k = 0; k < 10; k++)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Work: " << k << " " << std::this_thread::get_id() << std::endl;
		progress(k / 10.0);
	}
	done(5);
	std::cout << "Done" << std::endl;

}

void progress(double progress)
{
	std::cout << "Progress: " << std::this_thread::get_id() << std::endl;
}

void done(int result)
{
	std::cout << "Result: " << result << std::endl;
}

int main()
{
	bthread.Start(notify);
	bthread.AddTask(
		std::make_shared<Task<int>>(
			Task<int>(
				bthread,work,progress,done)
			)
	);
	
	std::string line;
	std::getline(std::cin, line);
	bthread.Stop();
	return 0;
}
