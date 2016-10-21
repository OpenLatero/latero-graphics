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

#ifndef LATERO_GRAPHICS_TIMER
#define LATERO_GRAPHICS_TIMER

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero {
namespace graphics {

class Timer
{
public:
	Timer()
	{
		Reset();
	}

	void Reset()
	{
		initTime_ = boost::posix_time::microsec_clock::universal_time();
	}

    inline boost::posix_time::time_duration GetElapsed() const
    {
        return (boost::posix_time::microsec_clock::universal_time() - initTime_);
    }
    
	inline double GetElapsedMicro() const
	{
		return (boost::posix_time::microsec_clock::universal_time() - initTime_).total_microseconds();
	}

	inline double GetElapsedSecond() const
	{
		return (boost::posix_time::microsec_clock::universal_time() - initTime_).total_microseconds() / 1E6;
	}

protected:
	boost::posix_time::ptime initTime_;
};

} // namespace graphics
} // namespace latero

#endif
