#include <future>
#include <iostream>
#include <string>
#include <sstream>
struct Result {
	int a;
	int b;
	int c;
	std::string to_string()
	{
		std::stringstream ss("Result{");
		ss << a << ", " << b << ", " << c;
		return ss.str();
	}
};

void work(std::function<void(double)> progress, std::function<void(Result)> done)
{
	std::cout << "Work: " << std::this_thread::get_id() << std::endl;
	for (int k = 0; k < 10; k++)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Work: " << k << " " << std::this_thread::get_id() << std::endl;
		progress(k / 10.0);
	}
	done(Result{ 1,2,3 });
	std::cout << "Done" << std::endl;

}

void progress(double progress)
{
	std::cout << "Progress: " << std::this_thread::get_id() << std::endl;
}

void done(Result result)
{
	std::cout << result.to_string() << std::endl;
}

int main()
{
	std::promise<double> result;

	std::cout << "Main: " << std::this_thread::get_id() << std::endl;
	
	std::thread(std::bind(work, progress, done)).detach();

	std::string line;
	std::getline(std::cin, line);
}