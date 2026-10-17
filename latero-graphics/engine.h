#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

namespace latero::graphics {

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
	 * engines, instead of the default steps of about 15.6 ms.
	 */
	void SleepUntil(std::chrono::steady_clock::time_point deadline);
private:
	/** body of the thread: gives it real-time priority if high-priority, then runs EntryPoint() */
	void Run();

	bool enabled_;
	std::shared_ptr<std::thread> thread_;
	std::atomic<bool> stop_{false};
	bool highPriority_;
};

} // namespace
