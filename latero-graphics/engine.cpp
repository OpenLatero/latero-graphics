#include "engine.h"
#include <assert.h>
#include <stdio.h>
#ifdef _WIN32
#include <optional>
#include "wintiming.h"
#endif

namespace latero::graphics {

Engine::Engine(bool highPriority) :
	enabled_(true), highPriority_(highPriority)
{
}

Engine::~Engine()
{
}
	
int Engine::Start()
{
	assert(!thread_);
	stop_ = false;
	thread_ = std::make_shared<std::thread>(&Engine::Run, this);
	return 0;
}

void Engine::Run()
{
#ifdef _WIN32
	// Windows needs help to run a high-priority loop on time; see wintiming.h.
	std::optional<win::RealtimeThread> realtimeThread;
	std::optional<win::PreciseSleeper> sleeper;
	if (highPriority_)
	{
		realtimeThread.emplace();
		sleeper.emplace();
		sleeper_ = &*sleeper;
	}
	EntryPoint();
	sleeper_ = nullptr;
#else
	if (highPriority_) SetHighPriority();
	EntryPoint();
#endif
}

void Engine::SleepUntil(std::chrono::steady_clock::time_point deadline)
{
#ifdef _WIN32
	if (sleeper_)
	{
		sleeper_->SleepUntil(deadline);
		return;
	}
#endif
	std::this_thread::sleep_until(deadline);
}

void Engine::SetHighPriority()
{
	// std::thread doesn't provide any support for priorities. We
	// therefore have to use non-portable native functions on the
	// calling thread.
	//
	// @todo These calls fail unless the program is running as
	// root. This should be handled better.
	//
	// @note This code was taken from http://stackoverflow.com/questions/1479945

	pthread_t threadID = pthread_self();
	int policy;
	struct sched_param param;
	int rv = pthread_getschedparam(threadID, &policy, &param);
	if (rv)
	{
		errno = rv;
	    perror("pthread_getschedparam");
	}

	policy = SCHED_FIFO;
	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	rv = pthread_setschedparam(threadID, policy, &param);
	if (rv)
	{
		errno = rv;
		perror("pthread_setschedparam: could not set real-time priority (try running as root)");
	}
}


void Engine::Stop()
{
	if (!thread_) return;
	stop_ = true;
	thread_->join();
}

} // namespace
