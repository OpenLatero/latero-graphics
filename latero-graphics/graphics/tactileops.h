#pragma once

namespace latero::graphics {

// Todo: are these making the code easier to grasp or these operations more obscure?
class TactileOps
{
public:
	static inline double Blend(double src, double blended, double alpha)
	{
		return (1-alpha)*src + alpha*blended;
	}

	static inline double WeightedSum(double src, double added, double weight)
	{
		// note: same as blending!
		return (1-weight)*src + weight*added;
	}

	static inline double Mask(double src, double mask)
	{
		return src * mask;
	}

};

} // namespace

