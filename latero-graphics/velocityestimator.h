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

#ifndef LATERO_GRAPHICS_VELOCITY_ESTIMATOR_H
#define LATERO_GRAPHICS_VELOCITY_ESTIMATOR_H

#include <math.h>
#include "point.h"
#include "averagefilter.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero {
namespace graphics { 

class VelocityEstimator
{
public:
	// velocity is averaged over this number of samples
	static const int average_size = 50;

	// velocity is computed as the difference between position over this number of samples
	static const int window_size = 10;

	VelocityEstimator();

	virtual ~VelocityEstimator() {}

	// insert latest position of the device
	void Insert(const Point &pos, const boost::posix_time::time_duration &time);

	// @return estimated velocity as a vector
	Point GetVelocity();

	// magnitude of the estimated velocity in mm/s
	double GetMagnitude();

	// direction of the estimate velocity in rad (between 0 and 2*PI)
	double GetDirection();

protected:
	unsigned int oldest_;
	Point positions_[window_size];
	boost::posix_time::time_duration times_[window_size];
	AverageFilter<double> velx,vely; // cannot average direction since it's circular
};

} // namespace graphics
} // namespace latero

#endif
