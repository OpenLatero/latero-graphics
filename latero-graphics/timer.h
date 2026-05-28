#pragma once

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero::graphics {

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

} // namespace

