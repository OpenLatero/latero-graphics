#include "engine.h"
#include <assert.h>
#include <stdio.h>

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
	thread_ = std::make_shared<std::thread>(&Engine::EntryPoint, this);
	if (highPriority_) SetHighPriority();
	return 0;
}

void Engine::SetHighPriority()
{
	// std::thread doesn't provide any support for priorities. We
	// therefore have to use the native handle exposed by
	// native_handle() and non-portable native functions.
	//
	// @todo These calls fail unless the program is running as
	// root. This should be handled better.
	//
	// @note This code was taken from http://stackoverflow.com/questions/1479945

	if (!thread_) return;
	pthread_t threadID = thread_->native_handle();
	int policy;
	struct sched_param param;
	int rv = pthread_getschedparam(threadID, &policy, &param);
	if (rv)
	{
		errno = rv;
	    perror("pthread_getschedparam");
	}

	policy = SCHED_FIFO;
	param.sched_priority = 99;
	rv = pthread_setschedparam(threadID, policy, &param);
	if (rv)
	{
		errno = rv;
	 }
	    perror("pthread_setschedparam");
}


void Engine::Stop()
{
	if (!thread_) return;
	stop_ = true;
	thread_->join();
}

} // namespace
