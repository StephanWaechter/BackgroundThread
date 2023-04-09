// CMakeTest.cpp : Defines the entry point for the application.
//

#include <BackgroundThread/BackgroundThread.hpp>
#include <iostream>
using namespace std;

BackgroundThread bthread;
void notify()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	bthread.Continue();
}

int main()
{
	bthread.Start(notify);
	ptrBackgroundTaskBase task = std::make_shared<BackgroundTask<void>>(
		BackgroundTask<void>(
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
