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

#include "checkframe.h"

namespace latero {
namespace graphics { 

namespace gtk {

CheckFrame::CheckFrame(bool checkState, std::string label, bool check_in_label) :
	Gtk::Frame(label)
{
	if (check_in_label)
	{
		check_.set_label(label);
		set_label_widget(check_);
	}

	check_.set_active(checkState);
	innerBox_.set_sensitive(checkState);

	add(mainBox_);
	if (!check_in_label) mainBox_.pack_start(check_, Gtk::PACK_SHRINK);
	mainBox_.pack_start(innerBox_);
	check_.signal_clicked().connect(sigc::mem_fun(*this, &CheckFrame::OnClick));
}


} // namespace gtk

} // namespace graphics
} // namespace latero

