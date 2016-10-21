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

#ifndef LATERO_GRAPHICS_PLANAR_MODULATOR_COMBO
#define LATERO_GRAPHICS_PLANAR_MODULATOR_COMBO

#include "modulatorfwd.h"
#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>
#include <latero/tactograph.h>

namespace latero {
namespace graphics { 

class ModulatorCombo : public Gtk::ComboBox
{
public:
	ModulatorCombo(ModulatorPtr tx);
	virtual ~ModulatorCombo() {};
	void Append(ModulatorPtr mod);
	void SetActive(ModulatorPtr mod);
	sigc::signal<void,ModulatorPtr> SignalModulatorChanged() { return signalModulatorChanged_; }

protected:
	sigc::signal<void,ModulatorPtr> signalModulatorChanged_;
	void OnComboChanged();

	class Columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Columns() { add(img); add(mod); }
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > img;
		Gtk::TreeModelColumn<ModulatorPtr> mod;
	};

	const latero::Tactograph *dev_; // tmp?
	Columns columns_;
	Glib::RefPtr<Gtk::ListStore> model_;
	bool signalEnable_;
};

} // namespace graphics
} // namespace latero

#endif

