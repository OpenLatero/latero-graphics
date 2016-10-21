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

#ifndef LATERO_GRAPHICS_AVERAGE_FILTER_H
#define LATERO_GRAPHICS_AVERAGE_FILTER_H

#include <stdio.h>

namespace latero {
namespace graphics {

template<class TYPE>
class AverageFilter
{
public:
	AverageFilter() : 
		wndSize_(0)
	{
		Reset();
	};

	~AverageFilter()
	{
		Reset();
	} 

	void Reset()
	{
		if (wndSize_) delete[] hist_;
		sum_= 0;
		n_ = 0;
		i_ = 0;
		first_ = 0;
	}

	// resets the filter (not ideal but simple)
	void SetSize(unsigned int size)
	{
		Reset();
		hist_ = new TYPE[size];
		wndSize_ = size;
		//printf("SetSize(%d): sum_(%f), first_(%d), i_(%d), n_(%d), avg(%f)\n",
		//	size, sum_, first_, i_, n_, Get());
	};
	
	void Insert(TYPE value)
	{
		if (n_ == wndSize_)
		{
			sum_ -= hist_[first_];
			first_ = (first_+1)%wndSize_;
		}
		else
		{
			n_++;
		}

		sum_ += value;
		hist_[i_] = value;
		i_ = (i_+1)%wndSize_;
	

		//printf("Insert(%f): sum_(%f), first_(%d), i_(%d), n_(%d), avg(%f)\n",
		//	(float)value, sum_, first_, i_, n_, Get());
	}

	inline double Get() { return (n_) ? sum_/n_ : 0; }
		
private:
	unsigned int i_; // current pointer location
	unsigned int first_;
	unsigned int n_; // number of items in the list
	TYPE *hist_;
	double sum_;
	unsigned int wndSize_;
};

//static void AverageFilterSelfTest();

} // namespace graphics
} // namespace latero

#endif
