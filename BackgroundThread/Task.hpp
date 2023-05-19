#pragma once
#include <future>
#include "UiWork.hpp"

namespace BackgroundThread
{
	class Task
	{
	public:
		void Execute() { pimpl.get()->Execute(); };
		UiWork GetUiWork() { return pimpl.get()->GetUiWork(); };
		bool Completed() { return pimpl.get()->Completed(); };

		template <typename TResult>
		Task(std::function<TResult(void)> work) :
			pimpl{ std::make_unique<TaskModel<TResult>>(work) }
		{}

		template <typename TResult>
		static Task Create(std::function<TResult(void)> work) { return Task(work); }

		template <typename TResult>
		Task(
			std::function<TResult(void)> work,
			std::function<void(std::future<TResult>)> done
		) :
			pimpl{ std::make_unique<TaskModel<TResult>>(work, done) }
		{}

		template <typename TResult>
		static Task Create(
			std::function<TResult(void)> work,
			std::function<void(std::future<TResult>)> done
		) {
			return Task(work, done);
		}

		Task() :
			pimpl{ std::make_unique<TaskModel<void>>() }
		{}

		Task(Task&& other) noexcept : pimpl{ std::move(other.pimpl) }  {}
		Task& operator=(Task&& other) noexcept
		{
			std::swap(pimpl, other.pimpl);
			return *this;
		};

	private:
		struct TaskConcept
		{
			virtual void Execute() = 0;
			virtual UiWork GetUiWork() = 0;
			virtual bool Completed() = 0;
		};

		struct DoNothingModel : TaskConcept
		{
			void Execute() {};
			UiWork GetUiWork() { return UiWork(); };
			bool Completed() { return false; };
		};

		template<class TResult>
		struct TaskModel : public TaskConcept
		{
			using fwork = std::function<TResult(void)>;
			using fdone = std::function<void(std::future<TResult>)>;
			TaskModel() : m_work{}, m_done{}, m_aborted{ true }
			{}

			TaskModel(
				fwork work
			) : m_work{ work }, m_done{}, m_aborted{ true }
			{}

			TaskModel(
				fwork work,
				fdone done
			) : m_work{ work }, m_done{ done }, m_aborted{ false }
			{}

			bool Completed() override { return !m_aborted; }
			void Execute() override
			{
				try
				{
					if constexpr (std::is_void<TResult>())
					{
						m_work();
						m_promis.set_value();
					}
					else
					{
						TResult result = m_work();
						m_promis.set_value(result);
					}
				}
				catch (AbortedException)
				{
					m_aborted = true;
				}
				catch (...)
				{
					m_promis.set_exception(
						std::current_exception()
					);
				}
			}

			UiWork GetUiWork()
			{
				return UiWork(
					m_done,
					m_promis.get_future()
				);
			}

			std::promise<TResult> m_promis;
			bool m_aborted{ false };
			fwork m_work;
			fdone m_done;
		};

		std::unique_ptr<TaskConcept> pimpl;
	};
}