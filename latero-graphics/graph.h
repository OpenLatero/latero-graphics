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

#ifndef LATERO_GRAPHICS_PLANAR_GRAPH_H
#define LATERO_GRAPHICS_PLANAR_GRAPH_H

#include "gtk/plot.h"
#include <gtkmm/box.h>
#include <gtkmm/eventbox.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero {
namespace graphics { 

class Graph : public Gtk::EventBox
{
public:
	Graph(uint nbPoints=1000, const char *fgColor = "blue", const char *bgColor = "white");
	virtual ~Graph();
	void Refresh();
	virtual void UpdateRange() {};
protected:
	virtual double GetValue(double x) = 0;

	boost::posix_time::ptime refreshTime_; // time of the last refresh
	uint nbPoints_;
	latero::graphics::gtk::Plot plot_;
};

} // namespace graphics
} // namespace latero

#endif
