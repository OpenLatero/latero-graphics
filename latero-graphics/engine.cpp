#include "engine.h"
#include <assert.h>
#include <stdio.h>
#include <optional>
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // Windows 10 and 11, for SetProcessInformation and power throttling
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <avrt.h>
#include <immintrin.h>
// Not defined by older MinGW headers.
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
#define PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION 0x4
#endif
#else
#include <errno.h>
#include <pthread.h>
#endif

namespace latero::graphics {

namespace {

#ifdef _WIN32
/**
 * By default, Windows wakes sleeping threads in steps of about 15.6 ms, far too coarse for a 500 to
 * 1000 Hz loop. High-priority engines therefore wait on a high-resolution timer (one per thread, while
 * it runs) until shortly before the deadline, then spin for the last spinMargin, which the timer alone
 * cannot guarantee. The spin costs CPU time, at most spinMargin per wait.
 */
thread_local HANDLE preciseTimer = nullptr;
constexpr std::chrono::microseconds spinMargin{500};
#endif

/**
 * Gives the calling thread real-time priority for as long as the object exists.
 */
class RealtimeScope
{
public:
	RealtimeScope();
	~RealtimeScope();
	RealtimeScope(const RealtimeScope&) = delete;
	RealtimeScope& operator=(const RealtimeScope&) = delete;
#ifdef _WIN32
private:
	HANDLE mmcssHandle_ = nullptr;
#endif
};

#ifdef _WIN32

RealtimeScope::RealtimeScope()
{
	// Keep Windows from throttling the process (in particular its timer resolution) to save power,
	// which Windows 11 does when the window is minimized or hidden.
	PROCESS_POWER_THROTTLING_STATE throttling = {};
	throttling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
	throttling.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED | PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
	throttling.StateMask = 0; // i.e. throttling disabled for everything in ControlMask
	if (!SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &throttling, sizeof(throttling)))
		fprintf(stderr, "Warning: could not disable power throttling (error %lu)\n", GetLastError());

	// Register with the Multimedia Class Scheduler Service, as audio software does.
	DWORD taskIndex = 0;
	mmcssHandle_ = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
	if (mmcssHandle_)
	{
		AvSetMmThreadPriority(mmcssHandle_, AVRT_PRIORITY_HIGH);
		printf("Update thread registered with MMCSS (Pro Audio, high priority)\n");
	}
	else
	{
		fprintf(stderr, "Warning: could not register the update thread with MMCSS (error %lu), using time-critical priority instead\n", GetLastError());
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}

	preciseTimer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
	if (!preciseTimer)
		fprintf(stderr, "Warning: could not create a high-resolution timer (error %lu)\n", GetLastError());
}

RealtimeScope::~RealtimeScope()
{
	if (preciseTimer)
	{
		CloseHandle(preciseTimer);
		preciseTimer = nullptr;
	}
	if (mmcssHandle_)
		AvRevertMmThreadCharacteristics(mmcssHandle_);
}

#else

RealtimeScope::RealtimeScope()
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

RealtimeScope::~RealtimeScope()
{
}

#endif

} // namespace

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
	std::optional<RealtimeScope> realtime;
	if (highPriority_) realtime.emplace();
	EntryPoint();
}

void Engine::SleepUntil(std::chrono::steady_clock::time_point deadline)
{
#ifdef _WIN32
	if (preciseTimer)
	{
		using namespace std::chrono;

		// sleep until shortly before the deadline...
		auto remaining = deadline - steady_clock::now();
		if (remaining > spinMargin)
		{
			LARGE_INTEGER dueTime;
			dueTime.QuadPart = -duration_cast<duration<long long, std::ratio<1, 10000000>>>(remaining - spinMargin).count(); // relative, in 100 ns units
			if (SetWaitableTimer(preciseTimer, &dueTime, 0, nullptr, nullptr, FALSE))
				WaitForSingleObject(preciseTimer, INFINITE);
		}

		// ...then spin until the deadline
		while (steady_clock::now() < deadline)
			_mm_pause();
		return;
	}
#endif
	std::this_thread::sleep_until(deadline);
}

void Engine::Stop()
{
	if (!thread_) return;
	stop_ = true;
	thread_->join();
}

} // namespace
