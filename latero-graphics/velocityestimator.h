#pragma once

#include <math.h>
#include "point.h"
#include "averagefilter.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero::graphics {

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

} // namespace

