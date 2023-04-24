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

int work(int input,std::function<void(double)> progress, Token const * const token)
{
	std::cout << "Work: " << std::this_thread::get_id() << std::endl;
	for (int k = 0; k < 20; k++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		token->ThrowIfAborted();
		//std::cout << "Work: " << k << " " << std::this_thread::get_id() << std::endl;
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

void done2(std::shared_future<int> result)
{
	try
	{
		std::cout << "Result2: " << result.get() << std::endl;
	}
	catch(const AbortedException)
	{
		std::cout << "Thread2 was aborted" << std::endl;
	}
}

int main()
{
	auto token2 = std::make_shared<Token>();
	auto token = std::make_shared<Token>();
	bthread = std::make_unique<Thread>(notify, 1);


	for(int k = 0; k < 1; k++)
	{
		auto task = CreateTask<int>(
			[=] (std::function<void(double)> progress, Token const * const token) -> int
			{
				return work(k, progress, token);
			},
			progress,
			done,
			token);
		bthread->Run(task);
	}

	auto task = CreateTask<int>(
		[=] (std::function<void(double)> progress, Token const * const token) -> int
		{
			return work(-1, progress, token);
		},
		progress,
		done2,
		token2);
	token2->Abort();
	bthread->Run(task);
	

	std::string line;
	return 0;
}
