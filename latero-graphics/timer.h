#pragma once

#include <chrono>

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
		initTime_ = std::chrono::steady_clock::now();
	}

    inline std::chrono::microseconds GetElapsed() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - initTime_);
    }

	inline double GetElapsedMicro() const
	{
		return GetElapsed().count();
	}

	inline double GetElapsedSecond() const
	{
		return GetElapsed().count() / 1E6;
	}

protected:
	std::chrono::steady_clock::time_point initTime_;
};

} // namespace
