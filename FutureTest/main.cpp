#include <future>
#include <iostream>
#include <string>
#include <sstream>

void printResult(std::future<std::string> future)
{
	std::cout << future.get() << std::endl;
}

int main()
{
	auto future = std::async(
		std::launch::deferred, 
		[]() -> std::string 
		{ 
			return std::string{ "Hello World!!!" }; 
		}
	);

	auto finsih = [onDone = printResult, f = std::move(future)] () mutable
	{
		onDone(std::move(f));
	};

	finsih();

}