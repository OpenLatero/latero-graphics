#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // Windows 10, for SetProcessInformation and power throttling
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <avrt.h>
#include <timeapi.h>
#include <immintrin.h>
#include <cstdio>
#include "wintiming.h"

// Not defined by older Windows SDK and MinGW headers.
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
#define PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION 0x4
#endif

namespace latero::graphics::win {

using namespace std::chrono;

/***** RealtimeThread *****/

RealtimeThread::RealtimeThread()
{
	// Keep Windows from throttling the process (in particular its timer resolution) to save power,
	// which Windows 11 does when the window is minimized or hidden.
	PROCESS_POWER_THROTTLING_STATE throttling = {};
	throttling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
	throttling.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED | PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
	throttling.StateMask = 0; // i.e. throttling disabled for everything in ControlMask
	if (!SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &throttling, sizeof(throttling)))
		fprintf(stderr, "Warning: could not disable power throttling (error %lu)\n", GetLastError());

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
}

RealtimeThread::~RealtimeThread()
{
	if (mmcssHandle_)
		AvRevertMmThreadCharacteristics(mmcssHandle_);
}

/***** PreciseSleeper *****/

PreciseSleeper::PreciseSleeper()
{
	// High-resolution timers (Windows 10 1803 and later) wake up with sub-millisecond precision
	// without changing the system-wide timer resolution.
	timer_ = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
	highResolution_ = (timer_ != nullptr);
	if (!highResolution_)
	{
		// Fall back on a regular timer, with the timer resolution raised to 1 ms.
		fprintf(stderr, "Warning: high-resolution timers not available, falling back on 1 ms timer resolution\n");
		timeBeginPeriod(1);
		timer_ = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
	}
}

PreciseSleeper::~PreciseSleeper()
{
	if (timer_)
		CloseHandle(timer_);
	if (!highResolution_)
		timeEndPeriod(1);
}

void PreciseSleeper::SleepUntil(steady_clock::time_point deadline)
{
	// sleep until shortly before the deadline...
	auto remaining = deadline - steady_clock::now();
	if (timer_ && remaining > spinMargin)
	{
		LARGE_INTEGER dueTime;
		dueTime.QuadPart = -duration_cast<duration<long long, std::ratio<1, 10000000>>>(remaining - spinMargin).count(); // relative, in 100 ns units
		if (SetWaitableTimer(timer_, &dueTime, 0, nullptr, nullptr, FALSE))
			WaitForSingleObject(timer_, INFINITE);
	}

	// ...then spin until the deadline
	while (steady_clock::now() < deadline)
		_mm_pause();
}

} // namespace

#endif
