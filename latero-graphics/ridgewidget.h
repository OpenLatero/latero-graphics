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

#ifndef LATERO_GRAPHICS_PLANAR_RIDGE_WIDGET_H
#define LATERO_GRAPHICS_PLANAR_RIDGE_WIDGET_H

#include <gtkmm/checkbutton.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/frame.h>
#include "gtk/checkframe.h"
#include "ridgefwd.h"
#include "ridgegraph.h"

namespace latero {
namespace graphics { 


class RidgeEdgeWidthScale : public Gtk::VBox
{
public:
	RidgeEdgeWidthScale(RidgePtr peer);
	virtual ~RidgeEdgeWidthScale() {};
private:
	void OnChange();
	RidgePtr peer_;
	Gtk::Adjustment adj_;
};

class RidgeTextureCtrl : public gtk::CheckFrame
{
public:
	RidgeTextureCtrl(RidgePtr peer);
	virtual ~RidgeTextureCtrl() {};
protected:
	void OnClick();
	RidgePtr peer_;
};


class RidgeControls : public Gtk::VBox
{
public:
	RidgeControls(RidgePtr peer);
protected:
	RidgePtr peer_;
	RidgeEdgeWidthScale edgeWidthScale_;
	RidgeTextureCtrl txCtrl_;
};


class RidgeWidget : public Gtk::HBox
{
public:
	RidgeWidget(RidgePtr peer);
protected:
	RidgeControls controls_;
	RidgePtr peer_;
	RidgeGraph *graph_;
};

} // namespace graphics
} // namespace latero

#endif
