// CMakeTest.cpp : Defines the entry point for the application.
//

#include <BackgroundThread/Thread.hpp>
#include <iostream>
using namespace std;
using namespace BackgroundThread;

Thread bthread;
void notify()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	bthread.Continue();
}

int main()
{
	bthread.Start(notify);
	auto task = std::make_shared<Task<void>>(
		Task<void>(
			[]()
			{
				std::cout << "Run: " << std::this_thread::get_id() << std::endl; return 10;
			}, 
			[](void)
			{ 
				std::cout << "Finalize: " << " " << std::this_thread::get_id() << std::endl;
			}
			)
		);
	bthread.Add(task);
	std::this_thread::sleep_for(std::chrono::seconds(4));
	return 0;
}
