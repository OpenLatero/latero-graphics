#include "velocityestimator.h"

namespace latero::graphics {

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

} // namespace

