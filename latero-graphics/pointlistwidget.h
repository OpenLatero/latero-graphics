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

#ifndef LATERO_GRAPHICS_POINT_LIST_WIDGET
#define LATERO_GRAPHICS_POINT_LIST_WIDGET

#include <gtkmm.h>
#include "point.h"

namespace latero {
namespace graphics { 

class PointListWidget : public Gtk::ScrolledWindow
{
public:
	PointListWidget(const std::vector<Point> &points);
	virtual ~PointListWidget();
	sigc::signal<void> SignalChanged() { return signalChanged_; };
	std::vector<Point> GetPoints();
protected:
	void InsertPoint(const Point &p);
	void OnDelete(int i);
	void OnInsert();
	void DeleteAdj();

	sigc::signal<void> signalChanged_;
	std::vector<Gtk::Adjustment*> xAdj_, yAdj_;
	std::vector<Gtk::HBox*> rowBox_;
	Gtk::VBox box_, pointBox_;
	Gtk::Button insertButton_;
};

} // namespace graphics
} // namespace latero

#endif
