// CMakeTest.cpp : Defines the entry point for the application.
//
#include <ThreadPool.hpp>
#include <Task.hpp>
#include <iostream>
#include <string>

using namespace BackgroundThread;

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
	ThreadPool threads(4);
	auto& uiworker = threads.get_UiWorker();
	uiworker.set_Notifer(
		[&uiworker] {
			uiworker.doWork();
		}
	);

	auto nprogress = uiworker.CreateNotifier<double>(
		[](double progress)
		{
			std::cout << ".";
		}
	);

	for(int k = 0; k < 4; k++)
	{
		auto task = Task::Create<int>(
			[=] () -> int
			{
				return work(k, nprogress);
			},
			done
			);
		threads.push(std::move(task));
	}

	std::string line;
	return 0;
}
