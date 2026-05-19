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

#include "modulatorfwd.h"
#include <gtkmm.h>
#include <latero/tactograph.h>

namespace latero::graphics {

class ModulatorDropDown : public Gtk::Box
{
public:
	ModulatorDropDown(ModulatorPtr tx);
	virtual ~ModulatorDropDown() {};
	void Append(ModulatorPtr mod);
	void SetActive(ModulatorPtr mod);
	sigc::signal<void(ModulatorPtr)> SignalModulatorChanged() { return signalModulatorChanged_; }

protected:
	sigc::signal<void(ModulatorPtr)> signalModulatorChanged_;
	void OnDropDownChanged();

	const latero::Tactograph *dev_; // tmp?
	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	std::vector<ModulatorPtr> modList_;
	std::vector<Glib::RefPtr<Gdk::Pixbuf>> imgList_;
	bool signalEnable_;
};

} // namespace

