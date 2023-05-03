// CMakeTest.cpp : Defines the entry point for the application.
//

#include <BackgroundThread/Thread.hpp>
#include <BackgroundThread/Task.hpp>
#include <iostream>
#include <string>

using namespace BackgroundThread;

std::unique_ptr<Thread> bthread;
void notify()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	bthread->DoUiWork();
}

int work(int input,std::function<void(double)> progress)
{
	std::cout << "Work: " << std::this_thread::get_id() << std::endl;
	for (int k = 0; k < 20; k++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	
	std::cout << "Done"  << std::endl;
	return input;
}

void progress(double progres)
{
	std::cout << "Progress: " << progress << " " << std::this_thread::get_id() << std::endl;
}

void done(std::shared_future<int> result)
{
	try
	{
		std::cout << "Result: " << result.get() << std::endl;
	}
	catch(const AbortedException)
	{
		std::cout << "Thread was aborted" << std::endl;
	}
}

int main()
{
	bthread = std::make_unique<Thread>(notify, 1);
    auto nprogress = bthread->CreateNotifier<double>(progress);

	for(int k = 0; k < 1; k++)
	{
		auto task = CreateTask<int>(
			[=] () -> int
			{
				return work(k, nprogress);
			},
			done
			);
		bthread->Run(task);
	}


	for(int k = 0; k < 1; k++)
	{
		auto task = CreateTask<void>(
			[=] () -> void
			{
				work(k, nprogress);
			},
			[=] (std::shared_future<void>)
			{
				std::cout << "done" << std::endl;
			}

			);
		bthread->Run(task);
	}


	std::string line;
	return 0;
}
