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

#include "pattern.h"
#include <gtkmm.h>
#include "texture/texturedropdown.h"

namespace latero::graphics {

class PatternCreatorDialog : public Gtk::Window
{
public:
	PatternCreatorDialog(const latero::Tactograph *dev);
	virtual ~PatternCreatorDialog() {};

	void OnDropDownChanged();
	PatternPtr CreatePattern();
	sigc::signal<void(int)>& signal_response() { return signalResponse_; }

protected:
	Glib::RefPtr<Gtk::StringList> typeList_;
	Gtk::DropDown* typeDropDown_;
	TextureDropDown txDropDown_;
	const latero::Tactograph *dev_;
	std::string loadedFile_;
	sigc::signal<void(int)> signalResponse_;
};

} // namespace

