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

#ifndef LATERO_GRAPHICS_STATE_H
#define LATERO_GRAPHICS_STATE_H

#include <latero/tactograph.h>
#include "point.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero {
namespace graphics {

class State
{
public:
	State(const latero::Tactograph *dev) :
        tick(0),
		actuatorPositions(dev->GetFrameSizeX(), dev->GetFrameSizeY())
	{
	}

	virtual ~State()
	{
	}

	unsigned long tick;
	Point pos;	// position within virtual surface (mm) 
	double angle;	// orientation (rad)
	latero::ActuatorImg<Point> actuatorPositions; // position of individual actuators within the virtual surface (mm)
	double velMag; // magnitude of the velocity (mm/s)
	double velDir; // direction of the velocity (rad) 

	inline boost::posix_time::time_duration GetTimeElapsed() const { return timeElapsed_; }
	inline void SetTimeElapsed(boost::posix_time::time_duration t) { timeElapsed_ = t; }

protected:
	boost::posix_time::time_duration timeElapsed_; // time since program was started - TODO: rename to simply "time"?
};


/**
 * Rendering mode. Allows rendering to be done in "simulation" modes that do nit change any memory-based
 * elements such as finite-state machines. sim_deflection shows vibration as a deflection. sim_vibration shows it as
 * random noise.
 */
class RenderMode
{
public:
	RenderMode(bool psim=false, bool pvib_as_noise=false, bool puse_state=false) : sim(psim), vib_as_noise(pvib_as_noise), use_state(puse_state) {}

	inline bool VibAsNoise() const { return sim && vib_as_noise; }
	inline bool StateChangeAllowed() const { return !sim; }
	inline bool IgnoreState() const { return sim && !use_state; }

	bool sim;
	bool vib_as_noise;
	bool use_state;
};

class ActuatorState
{
public:
	ActuatorState() :
		mode(RenderMode()), pos(-1,-1), relativePos(-1,-1), i(0), j(0),
        velMag(0), velDir(0),
        timeElapsed_(boost::posix_time::seconds(0)) 
	{
	}

	ActuatorState(const State &state) :
		mode(RenderMode()),
		pos(-1,-1), relativePos(-1,-1), i(0), j(0),
        velMag(state.velMag), velDir(state.velDir),
		timeElapsed_(state.GetTimeElapsed())    
	{
    }

	RenderMode mode;     
	Point pos; // position of the actuator in the workspace
	Point relativePos; // position of the actuator relative to the center of the display
	uint i,j;
	double velMag; // magnitude of the velocity (mm/s)
	double velDir; // direction of the velocity (rad) 
	inline boost::posix_time::time_duration GetTimeElapsed() const { return timeElapsed_; }
	inline void SetTimeElapsed(boost::posix_time::time_duration &t) { timeElapsed_ = t; }

protected:
	boost::posix_time::time_duration timeElapsed_;
};



} // namespace graphics
} // namespace latero

#endif

