#pragma once

/**
 * Windows-only support for the timing of the real-time update loop (see TactileEngine::EntryPoint).
 *
 * By default, Windows wakes sleeping threads in steps of about 15.6 ms, far too coarse for a 500 to
 * 1000 Hz loop. This file provides a precise wait, real-time scheduling for the loop's thread, and
 * statistics on the loop's timing. It is private to the library: it is not installed, and it keeps
 * windows.h out of the public headers.
 */
#ifdef _WIN32

#include <chrono>
#include <cstdint>
#include <vector>

namespace latero::graphics::win {

/**
 * Configures the calling thread for the real-time loop for as long as the object exists: registers it
 * with the Multimedia Class Scheduler Service ("Pro Audio" task, as used by audio software), or raises
 * its priority if that fails, and keeps Windows from throttling the process's timers to save power.
 */
class RealtimeThread
{
public:
	RealtimeThread();
	~RealtimeThread();
	RealtimeThread(const RealtimeThread&) = delete;
	RealtimeThread& operator=(const RealtimeThread&) = delete;
private:
	void *mmcssHandle_ = nullptr; // HANDLE
};

/**
 * Waits until a deadline with microsecond precision: sleeps on a high-resolution waitable timer until
 * shortly before the deadline, then busy-waits (spins) for the remainder, which the timer alone cannot
 * guarantee. The spin costs CPU time, at most spinMargin per wait.
 */
class PreciseSleeper
{
public:
	static constexpr std::chrono::microseconds spinMargin{500};

	PreciseSleeper();
	~PreciseSleeper();
	PreciseSleeper(const PreciseSleeper&) = delete;
	PreciseSleeper& operator=(const PreciseSleeper&) = delete;

	void SleepUntil(std::chrono::steady_clock::time_point deadline);
private:
	void *timer_ = nullptr; // HANDLE
	bool highResolution_ = false;
};

/**
 * Statistics on the timing of the loop. Record() is meant to be called once per cycle: it does not
 * allocate, lock or print, and costs a few arithmetic operations. Values are binned in 1 us steps up to
 * maxBinUs (larger ones are counted in the last bin, but the exact maximum is kept).
 */
class LoopStats
{
public:
	static constexpr int maxBinUs = 20000;

	explicit LoopStats(std::chrono::microseconds nominalPeriod);

	/**
	 * @param lateness  time between the scheduled start of the cycle and its actual start
	 * @param period    time since the start of the previous cycle
	 */
	void Record(std::chrono::steady_clock::duration lateness, std::chrono::steady_clock::duration period);

	/** Prints a summary to stdout. */
	void Print() const;
private:
	const int64_t nominalPeriodUs_;
	std::vector<uint32_t> latenessHist_;
	std::vector<uint32_t> periodHist_;
	uint64_t count_ = 0;
	uint64_t missed_ = 0; // cycles that started more than one period late
	int64_t sumPeriodUs_ = 0;
	int64_t maxLatenessUs_ = 0;
	int64_t maxPeriodUs_ = 0;
	int64_t minPeriodUs_ = INT64_MAX;
};

} // namespace

#endif
