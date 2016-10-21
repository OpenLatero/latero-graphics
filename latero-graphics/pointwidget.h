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

#ifndef LATERO_GRAPHICS_POINT_WIDGET
#define LATERO_GRAPHICS_POINT_WIDGET

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include "point.h"

namespace latero {
namespace graphics { 

class PointWidget : public Gtk::HBox
{
public:
	PointWidget(const Point &init, 
		double xlim_min=-100000, double xlim_max=100000, 
		double ylim_min=-100000, double ylim_max=100000,
		bool showSliders = true);
	Point GetValue();
	void SetValue(const Point &p);
	sigc::signal<void> SignalValueChanged();
protected:
	sigc::signal<void> signalValueChanged_;
	Gtk::Adjustment xAdj_, yAdj_;
};

} // namespace graphics
} // namespace latero

#endif
