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

#include "graph.h"

#define NB_POINTS 1000

namespace latero {
namespace graphics { 

Graph::Graph(uint nbPoints, const char *fgColor, const char *bgColor) :
	refreshTime_(boost::posix_time::min_date_time),
	nbPoints_(nbPoints),
	plot_(fgColor, bgColor)
{	
	add(plot_);
}

Graph::~Graph()
{
}

void Graph::Refresh()
{
	UpdateRange();
	refreshTime_ = boost::posix_time::microsec_clock::universal_time();

	plot_.Clear();
	float xMin = plot_.GetMinX();
	float xMax = plot_.GetMaxX();

	float incr = (xMax-xMin) / nbPoints_;
	for (float x=xMin; x<xMax; x+=incr)
	{
		plot_.InsertPoint(x, GetValue(x));
	}
	plot_.Draw();
}

} // namespace graphics
} // namespace latero
