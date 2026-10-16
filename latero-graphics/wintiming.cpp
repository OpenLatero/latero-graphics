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
#include <algorithm>
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

/***** LoopStats *****/

LoopStats::LoopStats(microseconds nominalPeriod) :
	nominalPeriodUs_(nominalPeriod.count()),
	latenessHist_(maxBinUs + 1, 0),
	periodHist_(maxBinUs + 1, 0)
{
}

void LoopStats::Record(steady_clock::duration lateness, steady_clock::duration period)
{
	int64_t lateUs = duration_cast<microseconds>(lateness).count();
	int64_t periodUs = duration_cast<microseconds>(period).count();
	if (lateUs < 0) lateUs = 0;
	if (periodUs < 0) periodUs = 0;

	++latenessHist_[std::min<int64_t>(lateUs, maxBinUs)];
	++periodHist_[std::min<int64_t>(periodUs, maxBinUs)];
	++count_;
	sumPeriodUs_ += periodUs;
	if (lateUs > nominalPeriodUs_) ++missed_;
	if (lateUs > maxLatenessUs_) maxLatenessUs_ = lateUs;
	if (periodUs > maxPeriodUs_) maxPeriodUs_ = periodUs;
	if (periodUs < minPeriodUs_) minPeriodUs_ = periodUs;
}

/** Value below which the given fraction of the recorded values lie. */
static int64_t Percentile(const std::vector<uint32_t> &hist, uint64_t count, double fraction)
{
	uint64_t target = (uint64_t)(fraction * count);
	uint64_t sum = 0;
	for (size_t i = 0; i < hist.size(); ++i)
	{
		sum += hist[i];
		if (sum > target) return (int64_t)i;
	}
	return (int64_t)hist.size() - 1;
}

void LoopStats::Print() const
{
	if (count_ == 0) return;
	double measuredRate = sumPeriodUs_ > 0 ? 1e6 * count_ / sumPeriodUs_ : 0;
	printf("Update loop timing, %llu cycles over %.1f s:\n",
			(unsigned long long)count_, sumPeriodUs_ / 1e6);
	printf("  target rate:   %8.1f Hz, i.e. one cycle every %lld us\n",
			1e6 / nominalPeriodUs_, (long long)nominalPeriodUs_);
	printf("  measured rate: %8.1f Hz on average\n", measuredRate);
	printf("  Time between the starts of consecutive cycles, ideally %lld us [us]:\n", (long long)nominalPeriodUs_);
	printf("      min %lld, median %lld, 99%% of cycles below %lld, 99.9%% below %lld, max %lld\n",
			(long long)minPeriodUs_,
			(long long)Percentile(periodHist_, count_, 0.5),
			(long long)Percentile(periodHist_, count_, 0.99),
			(long long)Percentile(periodHist_, count_, 0.999),
			(long long)maxPeriodUs_);
	printf("  Delay of each cycle's start past its scheduled time, ideally 0 [us]:\n");
	printf("      median %lld, 99%% of cycles below %lld, 99.9%% below %lld, max %lld\n",
			(long long)Percentile(latenessHist_, count_, 0.5),
			(long long)Percentile(latenessHist_, count_, 0.99),
			(long long)Percentile(latenessHist_, count_, 0.999),
			(long long)maxLatenessUs_);
	printf("  Cycles that started more than one full period (%lld us) late: %llu (%.3f%%)\n",
			(long long)nominalPeriodUs_, (unsigned long long)missed_, 100.0 * missed_ / count_);
	printf("  Note: the loop keeps to a fixed schedule, so after a late cycle the next ones start early\n"
	       "  to catch up, which is why the shortest time between cycles can be well below the target.\n"
	       "  The wait spins for its last %lld us for precision.\n",
			(long long)PreciseSleeper::spinMargin.count());
}

} // namespace

#endif
