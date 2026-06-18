#pragma once

#include <boost/thread.hpp>
#include <memory>

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
private:
	bool enabled_;
    std::shared_ptr<boost::thread> m_thread;
    bool highPriority_;
};

} // namespace
