// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#ifndef LATERO_GRAPHICS_MUTEX_H
#define LATERO_GRAPHICS_MUTEX_H

#include <boost/thread/recursive_mutex.hpp>

namespace latero {
namespace graphics { 

typedef boost::recursive_mutex Mutex;

class Context
{
public:
	static Mutex *GetLock() { return &lock_; }
protected:
	static Mutex lock_;
};


/** This DEFINE instantiates a scoped lock for the global mutex. */
#define LATERO_GRAPHICS_GUARD boost::recursive_mutex::scoped_lock laterographicsguard(*latero::graphics::Context::GetLock());

/** This DEFINE locks the global mutex. */
#define LATERO_GRAPHICS_LOCK latero::graphics::Context::GetLock()->lock();

/** This DEFINE unlocks the global mutex. */
#define LATERO_GRAPHICS_UNLOCK latero::graphics::Context::GetLock()->unlock();

} // namespace graphics
} // namespace latero

#endif
