#pragma once

/**
 * Windows-only support for the timing of the real-time update loop (see TactileEngine::EntryPoint).
 *
 * By default, Windows wakes sleeping threads in steps of about 15.6 ms, far too coarse for a 500 to
 * 1000 Hz loop. This file provides a precise wait and real-time scheduling for the loop's thread (see
 * tactileenginestats.h for statistics on the loop's timing). It is private to the library: it is not
 * installed, and it keeps windows.h out of the public headers.
 */
#ifdef _WIN32

#include <chrono>

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

} // namespace

#endif
