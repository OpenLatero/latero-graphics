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

#ifndef LATERO_GRAPHICS_GTK_SPACER_H
#define LATERO_GRAPHICS_GTK_SPACER_H

#include <gtkmm.h>

namespace latero {
namespace graphics { 

namespace gtk {

// TODO: is there a better way of making a spacer??? 

class Spacer : public Gtk::Alignment
{
public:
	Spacer(int w, int h) : Gtk::Alignment(0,0) { set_size_request(w,h); }
	virtual ~Spacer() {};
};

class VSpacer : public Spacer
{
public:
	VSpacer(int h) : Spacer(-1,h) {}
	virtual ~VSpacer() {};
};

class HSpacer : public Spacer
{
public:
	HSpacer(int w) : Spacer(w,-1) {}
	virtual ~HSpacer() {};
};

} // namespace gtk

} // namespace graphics
} // namespace latero

#endif
