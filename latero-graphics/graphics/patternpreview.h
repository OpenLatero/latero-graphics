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
#include "pattern.h"

namespace latero::graphics {

class PatternPreview : public Gtk::Box
{
public:
	static const Pattern::VizMode viz_mode = Pattern::viz_deflection_and_vibration;
	static const int height = 200;

	PatternPreview(PatternPtr peer);
	virtual ~PatternPreview() { if (popupMenu_) popupMenu_->unparent(); }
protected:
	void CreatePopupMenu();
	void OnSave();
	void OnClick(int n_press, double x, double y);
	bool OnTimer();
	void Refresh();

	boost::posix_time::ptime refreshTime_;
	Gtk::Picture img_;
	std::unique_ptr<Gtk::PopoverMenu> popupMenu_;
	PatternPtr peer_;
};

} // namespace

