#include <thread>
#include <mutex>
#include <functional>

class BackgroundTaskBase
{
public:
	virtual void Run() = 0;
	virtual void Finalize() = 0;
};

template<class TResult>
class BackgroundTask : public BackgroundTaskBase
{
public:
	BackgroundTask(
		std::function<TResult(void)> work,
		std::function<void(TResult)> finalize) :
		m_work{work},
		m_result{nullptr},
		m_finalize{finalize} {};

	BackgroundTask(const BackgroundTask& o) : 
		m_work{ o.m_work },
		m_finalize{ o.m_finalize },
		m_result{ nullptr } {};

	void Run();
	void Finalize();

private:
	std::unique_ptr<TResult> m_result;
	std::function<TResult(void)> m_work;
	std::function<void(TResult)> m_finalize;
};

template<>
class BackgroundTask<void> : public BackgroundTaskBase
{
public:
	BackgroundTask(
		std::function<void()> work,
		std::function<void()> finalize) :
		m_work{ work },
		m_finalize{ finalize } {};

	BackgroundTask(const BackgroundTask& o) :
		m_work{ o.m_work },
		m_finalize{ o.m_finalize } {};

	void Run();
	void Finalize();

private:
	std::function<void()> m_work;
	std::function<void()> m_finalize;
};

inline void BackgroundTask<void>::Run()
{
	m_work();
}

inline void BackgroundTask<void>::Finalize()
{
	m_finalize();
}

template<class TResult>
inline void BackgroundTask<TResult>::Run()
{
	m_result = std::make_unique<TResult>( m_work() );
}

template<class TResult>
inline void BackgroundTask<TResult>::Finalize()
{
	m_finalize(*m_result);
}

