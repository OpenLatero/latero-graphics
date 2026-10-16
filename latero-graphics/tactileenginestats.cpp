#include <cstdio>
#include <algorithm>
#include "tactileenginestats.h"

namespace latero::graphics {

using namespace std::chrono;

TactileEngineStats::TactileEngineStats(microseconds nominalPeriod) :
	nominalPeriodUs_(nominalPeriod.count()),
	latenessHist_(maxBinUs + 1, 0),
	periodHist_(maxBinUs + 1, 0)
{
}

void TactileEngineStats::Record(steady_clock::duration lateness, steady_clock::duration period)
{
	int64_t lateUs = duration_cast<microseconds>(lateness).count();
	int64_t periodUs = duration_cast<microseconds>(period).count();
	if (lateUs < 0) lateUs = 0;
	if (periodUs < 0) periodUs = 0;

	++latenessHist_[std::min<int64_t>(lateUs, maxBinUs)];
	++periodHist_[std::min<int64_t>(periodUs, maxBinUs)];
	++count_;
	sumPeriodUs_ += periodUs;
	if (lateUs > nominalPeriodUs_) ++missed_;
	if (lateUs > maxLatenessUs_) maxLatenessUs_ = lateUs;
	if (periodUs > maxPeriodUs_) maxPeriodUs_ = periodUs;
	if (periodUs < minPeriodUs_) minPeriodUs_ = periodUs;
}

/** Value below which the given fraction of the recorded values lie. */
static int64_t Percentile(const std::vector<uint32_t> &hist, uint64_t count, double fraction)
{
	uint64_t target = (uint64_t)(fraction * count);
	uint64_t sum = 0;
	for (size_t i = 0; i < hist.size(); ++i)
	{
		sum += hist[i];
		if (sum > target) return (int64_t)i;
	}
	return (int64_t)hist.size() - 1;
}

void TactileEngineStats::Print() const
{
	if (count_ == 0) return;
	double measuredRate = sumPeriodUs_ > 0 ? 1e6 * count_ / sumPeriodUs_ : 0;
	printf("Update loop timing, %llu cycles over %.1f s:\n",
			(unsigned long long)count_, sumPeriodUs_ / 1e6);
	printf("  target rate:   %8.1f Hz, i.e. one cycle every %lld us\n",
			1e6 / nominalPeriodUs_, (long long)nominalPeriodUs_);
	printf("  measured rate: %8.1f Hz on average\n", measuredRate);
	printf("  Time between the starts of consecutive cycles, ideally %lld us [us]:\n", (long long)nominalPeriodUs_);
	printf("      min %lld, median %lld, 99%% of cycles below %lld, 99.9%% below %lld, max %lld\n",
			(long long)minPeriodUs_,
			(long long)Percentile(periodHist_, count_, 0.5),
			(long long)Percentile(periodHist_, count_, 0.99),
			(long long)Percentile(periodHist_, count_, 0.999),
			(long long)maxPeriodUs_);
	printf("  Delay of each cycle's start past its scheduled time, ideally 0 [us]:\n");
	printf("      median %lld, 99%% of cycles below %lld, 99.9%% below %lld, max %lld\n",
			(long long)Percentile(latenessHist_, count_, 0.5),
			(long long)Percentile(latenessHist_, count_, 0.99),
			(long long)Percentile(latenessHist_, count_, 0.999),
			(long long)maxLatenessUs_);
	printf("  Cycles that started more than one full period (%lld us) late: %llu (%.3f%%)\n",
			(long long)nominalPeriodUs_, (unsigned long long)missed_, 100.0 * missed_ / count_);
	printf("  Note: the loop keeps to a fixed schedule, so after a late cycle the next ones start early\n"
	       "  to catch up, which is why the shortest time between cycles can be well below the target.\n");
}

} // namespace
