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

#ifndef LATERO_GRAPHICS_ENGINE_H
#define LATERO_GRAPHICS_ENGINE_H

#include <boost/thread.hpp>

namespace latero {
namespace graphics {

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
    boost::shared_ptr<boost::thread> m_thread;
    bool highPriority_;
};

} // namespace graphics
} // namespace latero

#endif
