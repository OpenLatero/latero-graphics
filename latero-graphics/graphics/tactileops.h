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

#ifndef LATERO_GRAPHICS_GRAPHICS_TACTILE_OPS_H
#define LATERO_GRAPHICS_GRAPHICS_TACTILE_OPS_H

namespace latero {
namespace graphics { 

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

} // namespace graphics
} // namespace latero

#endif
