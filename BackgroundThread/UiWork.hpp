#pragma once
#include <functional>
#include <future>
#include <memory>

namespace BackgroundThread
{
	class UiWork
	{
	private:
		struct UiWorkConcept
		{
			virtual void Exceute() = 0;
		};

		struct DoNothingModel : public UiWorkConcept
		{
			DoNothingModel() {}
			void Exceute() override {}
		};

		struct NotifierModel : public UiWorkConcept
		{
			NotifierModel(
				std::function<void(void)> done
			) : 
				m_done{ done } 
			{}
			void Exceute() override
			{
				m_done();
			}
			std::function<void(void)> m_done;
		};

		template<class TResult>
		struct ResultModel : public UiWorkConcept
		{
			ResultModel(
				std::function<void(std::future<TResult>)> done,
				std::future<TResult> result
			) :
				m_done{ done },
				m_result{ std::move(result) }

			{}

			void Exceute() override
			{
				m_done(std::move(m_result));
			}

			std::future<TResult> m_result;
			std::function<void(std::future<TResult>)> m_done;
		};

		std::unique_ptr<UiWorkConcept> pimpl;

	public:
		void Exceute() { pimpl->Exceute(); }

		template<typename TResult>
		UiWork(std::function<void(std::future<TResult>)> done, std::future<TResult> result)
			:
			pimpl{ std::make_unique<ResultModel<TResult>>(done, std::move(result)) }
		{}

		UiWork(std::function<void(void)> done) :
			pimpl{ std::make_unique<NotifierModel>(done) }
		{}

		UiWork() :
			pimpl{ std::make_unique<DoNothingModel>() }
		{}

		UiWork(UiWork&& other) noexcept : pimpl{ std::move(other.pimpl) } {}
		UiWork& operator=(UiWork&& other) noexcept { std::swap(pimpl, other.pimpl); return *this; };
	};
}