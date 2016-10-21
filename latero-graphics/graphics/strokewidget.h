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

#ifndef LATERO_GRAPHICS_PLANAR_STROKE_WIDGET
#define LATERO_GRAPHICS_PLANAR_STROKE_WIDGET

#include "strokemotionwidget.h"

#include "strokefwd.h"
#include <gtkmm/box.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/checkbutton.h>


namespace latero {
namespace graphics { 

/**
 * Profile of the stroke across the line.
 */
class StrokeProfileWidget : public Gtk::HBox
{
public:
	StrokeProfileWidget(StrokePtr peer);
	virtual ~StrokeProfileWidget() {};
};


/**
 * Fill texture
 */
class StrokeFillWidget : public Gtk::VBox
{
public:
	StrokeFillWidget(StrokePtr peer);
	virtual ~StrokeFillWidget() {};
};


/**
 * Dotted texture
 */
class StrokeDottedWidget : public Gtk::VBox
{
public:
	StrokeDottedWidget(StrokePtr peer);
	virtual ~StrokeDottedWidget() {};
};

} // namespace graphics
} // namespace latero

#endif
