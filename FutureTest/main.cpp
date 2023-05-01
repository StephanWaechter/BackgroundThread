#include <future>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>


class Task {
	std::o
};

int main()
{
	std::queue<std::function<void(void)>> task_queue;


	for(int k = 0; k< 20; k++)
	{
		auto task = std::packaged_task(
			[=]() -> int
			{
				std::this_thread::sleep_for(std::chrono::microseconds(500));
				return 10;
			});
		task_queue.push(task);
		auto result = task.get_future();
	}
	task();

	
	finsih();

}