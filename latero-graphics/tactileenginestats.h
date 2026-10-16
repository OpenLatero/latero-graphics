#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

namespace latero::graphics {

/**
 * Statistics on the timing of the real-time update loop of TactileEngine. Record() is meant to be called
 * once per cycle: it does not allocate, lock or print, and costs a few arithmetic operations. Values are
 * binned in 1 us steps up to maxBinUs (larger ones are counted in the last bin, but the exact maximum is
 * kept).
 */
class TactileEngineStats
{
public:
	static constexpr int maxBinUs = 20000;

	explicit TactileEngineStats(std::chrono::microseconds nominalPeriod);

	/**
	 * @param lateness  time between the scheduled start of the cycle and its actual start
	 * @param period    time since the start of the previous cycle
	 */
	void Record(std::chrono::steady_clock::duration lateness, std::chrono::steady_clock::duration period);

	/** Prints a summary to stdout. */
	void Print() const;
private:
	const int64_t nominalPeriodUs_;
	std::vector<uint32_t> latenessHist_;
	std::vector<uint32_t> periodHist_;
	uint64_t count_ = 0;
	uint64_t missed_ = 0; // cycles that started more than one period late
	int64_t sumPeriodUs_ = 0;
	int64_t maxLatenessUs_ = 0;
	int64_t maxPeriodUs_ = 0;
	int64_t minPeriodUs_ = INT64_MAX;
};

} // namespace
