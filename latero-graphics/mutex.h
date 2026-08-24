#pragma once

#include <mutex>

namespace latero::graphics {

typedef std::recursive_mutex Mutex;

class Context
{
public:
	static Mutex *GetLock() { return &lock_; }
protected:
	static Mutex lock_;
};


/** This DEFINE instantiates a scoped lock for the global mutex. */
#define LATERO_GRAPHICS_GUARD std::lock_guard<std::recursive_mutex> laterographicsguard(*latero::graphics::Context::GetLock());

/** This DEFINE locks the global mutex. */
#define LATERO_GRAPHICS_LOCK latero::graphics::Context::GetLock()->lock();

/** This DEFINE unlocks the global mutex. */
#define LATERO_GRAPHICS_UNLOCK latero::graphics::Context::GetLock()->unlock();

} // namespace

