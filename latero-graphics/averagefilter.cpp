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

#include "averagefilter.h"
#include <assert.h>

namespace latero {
namespace graphics {

/*
static void AverageFilterSelfTest()
{
	printf("Average Self Test\n");

	AverageFilter<int> avg;
	assert(avg.Get()==0);

	avg.SetSize(4);
	assert(avg.Get()==0);

	avg.Insert(1); // 1
	assert(avg.Get()==1);

	avg.Insert(2); // 1,2
	assert(avg.Get()==1.5);

	avg.Insert(3); // 1,2,3
	assert(avg.Get()==2);

	avg.Insert(4); // 1,2,3.4
	assert(avg.Get()==2.5);

	avg.Insert(5); // 2,3.4,5
	assert(avg.Get()==3.5);

	avg.Insert(6); // 3.4,5,6
	assert(avg.Get()==4.5);
	
	avg.SetSize(2); // 
	assert(avg.Get()==0);

	avg.Insert(7); // 7
	assert(avg.Get()==7);

	avg.Insert(8); // 7,8
	assert(avg.Get()==7.5);

	avg.Insert(9); // 8,9
	assert(avg.Get()==8.5);

	avg.SetSize(3); // 
	assert(avg.Get()==0);

	avg.Insert(10); // 10
	assert(avg.Get()==10);

	avg.Insert(11); // 10,11
	assert(avg.Get()==10.5);

	avg.Insert(12); // 10,11,12
	assert(avg.Get()==11);

	avg.Insert(13); // 11,12,13
	assert(avg.Get()==12);

	avg.Insert(14); // 12,13,14
	assert(avg.Get()==13);

	avg.Insert(15); // 13,14,15
	assert(avg.Get()==14);

	avg.Insert(16); // 14,15,16
	assert(avg.Get()==15);
}
*/ 

} // namespace graphics
} // namespace latero

