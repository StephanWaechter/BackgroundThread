#pragma once
#include <functional>
#include <memory>
#include <chrono>

namespace BackgroundThread
{
	namespace GtkDemo
	{
		int DelayedWork(int time_in_seconds, std::function<void(double)> progress, std::shared_ptr<BackgroundThread::Token> token)
		{
			std::cout << "Running " << time_in_seconds << " on thread: " << std::this_thread::get_id() << std::endl;
			int N = time_in_seconds * 10;
			for (int k = 0; k < N; k++)
			{
				progress(k / double(N));
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				token->ThrowIfAborted();
			}
			return (time_in_seconds);
		}

		class Worker
		{

		};
	}
}