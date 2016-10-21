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

#ifndef LATERO_GRAPHICS_POSITIONGEN_H
#define LATERO_GRAPHICS_POSITIONGEN_H

#include "generator.h"
#include "positiongenfwd.h"

namespace latero {
namespace graphics { 

/**
 * This class represents a Generator in which the deflection of an actuator is determined
 * by its position within the virtual surface. Each actuator is independent of the others, but the
 * deflection may vary depending on the actuator's location within the tactile display. This class implements
 * the basic DoRender_ method and relies on subclasses' Render_, PreRender_ and PostRender_ methods to implement
 * the actual deflection map.
 *
 * @ingroup GeneratorGroup 
 */
class PositionGen : public Generator
{
public:
	/** destructor */
	virtual ~PositionGen();

	/**
	 * Returns an image in which each pixel corresponds to the deflection of an actuator when at that position
	 * within the virtual surface. This is the most accurate representation of what happens at the tactile
	 * level. It is currently computationally expensive (~30s), and therefore should be obtained only on
	 * occasion.
	 *
	 * @params w 		width in pixels of output image
	 * @params t 		time in ns (for time-varying generators)
	 * @param velMag	magnitude of the velocity of the display
	 * @param velDir	direction of the display movement
	 * @param mode		rendering mode
	 *
	 * @returns output image
	 *
	 * @warning returns mapping for the actuator in upper-left corner
	 */
	Glib::RefPtr<Gdk::Pixbuf> GetDeflectionMap(uint w, boost::posix_time::time_duration timeElapsed=boost::posix_time::seconds(0), double velMag=0, double velDir=0, RenderMode mode = RenderMode(true,true));

protected:
	/** constructor */
	PositionGen(const latero::Tactograph *dev);

	// see Generator
	virtual void DoRender_(const State *state, latero::BiasedImg &frame);

	/** called before rendering individual actuators */
	virtual void PreRender_(const State *state) {};

	/**
	 * @stateless ignore effect of PreRender_ and PostRender_ (hack for GetDeflectionMap)
	 * @returns deflection (0-right to 1-left) of a given actuator (i,j) when at a specific location
	 *  within the virtual surface
	 * @warning non-locking
	 */
	virtual double Render_(const ActuatorState &state, bool stateless=false) = 0;

	/** called ater rendering individual actuators */
	virtual void PostRender_(const State *state) {};

};

} // namespace graphics
} // namespace latero

#endif
