#pragma once

#include <atomic>
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

	void SetHighPriority();

	/** function in which thread should run */
	virtual void EntryPoint() = 0;

	void SetEnabled(bool v=true) { enabled_ = v; }
	bool GetEnabled() { return enabled_; }
protected:
	bool ShouldStop() const { return stop_; }
private:
	bool enabled_;
	std::shared_ptr<std::thread> thread_;
	std::atomic<bool> stop_{false};
	bool highPriority_;
};

} // namespace
