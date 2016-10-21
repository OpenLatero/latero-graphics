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

#include "velocityestimator.h"

namespace latero {
namespace graphics { 

VelocityEstimator::VelocityEstimator()
{
	// reset the history to 0
	for (int i=0; i<window_size; ++i)
    {
		positions_[i] = Point(0,0);
        times_[i] = boost::posix_time::milliseconds(0);
    }
	oldest_=0;
	velx.SetSize(average_size);
	vely.SetSize(average_size);
}

void VelocityEstimator::Insert(const Point &pos, const boost::posix_time::time_duration &time)
{
	positions_[oldest_] = pos; 			// override oldest value
    times_[oldest_] = time;
	oldest_=(oldest_+1)%window_size; 	// move oldest pointer

	// compute velocity
	double seconds = (time - times_[oldest_]).total_microseconds() / 1E6;
	Point vel = (seconds > 0) ? ((pos - positions_[oldest_])/seconds) : Point(0,0);

	// compute averages
	velx.Insert(vel.x);
	vely.Insert(vel.y);
}

Point VelocityEstimator::GetVelocity()
{
	return Point(velx.Get(),vely.Get());
}

double VelocityEstimator::GetMagnitude()
{
	return GetVelocity().Norm();
}

double VelocityEstimator::GetDirection()
{
	Point vel = GetVelocity();
	double angle = atan2(vel.y,vel.x);
	if (angle<0) angle += 2*M_PI;
	return angle;
}

} // namespace graphics
} // namespace latero

