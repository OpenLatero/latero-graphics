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

#pragma once

#include <gtkmm.h>
#include "dotpatternfwd.h"

namespace latero::graphics {

class DotPatternMinSpacingCtrl : public Gtk::Box
{
public:
	DotPatternMinSpacingCtrl(DotPatternPtr peer);
	virtual ~DotPatternMinSpacingCtrl() {}
protected:
	void OnChanged();
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DotPatternPtr peer_;
};


class DotPatternWidget : public Gtk::Box
{
public:
	DotPatternWidget(DotPatternPtr peer);
	virtual ~DotPatternWidget() {};
};

} // namespace

