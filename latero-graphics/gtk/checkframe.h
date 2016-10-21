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

#ifndef LATERO_GRAPHICS_GTK_CHECK_FRAME_H
#define LATERO_GRAPHICS_GTK_CHECK_FRAME_H

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/checkbutton.h>

namespace latero {
namespace graphics { 

namespace gtk {

class CheckFrame : public Gtk::Frame
{
public:
	CheckFrame(bool checkState, std::string label = "", bool check_in_label = false);
	virtual ~CheckFrame() {};

	Gtk::CheckButton &GetCheck() { return check_; }
	Gtk::HBox &GetBox() { return innerBox_; }

protected:
	void OnClick() { innerBox_.set_sensitive(check_.get_active()); }

	Gtk::HBox mainBox_;
	Gtk::HBox innerBox_;
	Gtk::CheckButton check_;
};

} // namespace gtk

} // namespace graphics
} // namespace latero

#endif
