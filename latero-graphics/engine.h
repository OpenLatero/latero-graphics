#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

namespace latero::graphics {

namespace win { class PreciseSleeper; }

class Engine
{
public:
	Engine(bool highPriority = false);
	virtual ~Engine();

	int Start();
	void Stop();

	/** function in which thread should run */
	virtual void EntryPoint() = 0;

	void SetEnabled(bool v=true) { enabled_ = v; }
	bool GetEnabled() { return enabled_; }
protected:
	bool ShouldStop() const { return stop_; }

	/**
	 * Waits until the deadline. On Windows, the wait is precise to the microsecond for high-priority
	 * engines (see wintiming.h), instead of the default steps of about 15.6 ms.
	 */
	void SleepUntil(std::chrono::steady_clock::time_point deadline);
private:
	/** body of the thread: sets it up, then runs EntryPoint() */
	void Run();

	/** gives the calling thread real-time priority (macOS and Linux) */
	void SetHighPriority();

	bool enabled_;
	std::shared_ptr<std::thread> thread_;
	std::atomic<bool> stop_{false};
	bool highPriority_;
	win::PreciseSleeper *sleeper_ = nullptr; // Windows only, while a high-priority thread runs
};

} // namespace
