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

#ifndef LATERO_GRAPHICS_PLANAR_DIR_MODULATOR_WIDGET
#define LATERO_GRAPHICS_PLANAR_DIR_MODULATOR_WIDGET

#include <gtkmm/box.h>
#include "dirmodulatorfwd.h"

namespace latero {
namespace graphics { 

class DirModulatorWidget : public Gtk::HBox
{
public:
	DirModulatorWidget(DirModulatorPtr peer);
	virtual ~DirModulatorWidget() {};
protected:
	DirModulatorPtr peer_;
};

} // namespace graphics
} // namespace latero

#endif

