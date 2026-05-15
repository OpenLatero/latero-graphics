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

#ifndef LATERO_GRAPHICS_GTK_NUM_WIDGET_H
#define LATERO_GRAPHICS_GTK_NUM_WIDGET_H

#include <gtkmm.h>
#include <assert.h>
#include <sstream>
#include "../units.h"

namespace latero::graphics::gtk {

static const std::string units_default = "";
static const std::string name_default = "";


class NumWidget : public Gtk::Frame
{
public:
	NumWidget(Gtk::Orientation orient, Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string name = name_default, std::string units = units_default);
	virtual ~NumWidget() {}
	void AddUnits(std::string units, Glib::RefPtr<Gtk::Adjustment> adj, uint digits);
	void SelectUnits(std::string units);

	sigc::signal<void(std::string)> SignalUnitsChanged() { return signalUnitsChanged_; }
protected:
	sigc::signal<void(std::string)> signalUnitsChanged_;
	void SetDigits(uint n);
	void SetAdjustment(Glib::RefPtr<Gtk::Adjustment> adj);
	Glib::ustring OnFormat(double v);
	void OnUnitsChanged();

	Gtk::Box *unitsDropDownBox_;
	Gtk::DropDown *unitsDropDown_;
	Glib::RefPtr<Gtk::StringList> unitsStringList_;
	Gtk::SpinButton spin_;
	Gtk::Scale *scale_;

	std::string units_;
	std::map<std::string, uint> unitsToDigitsMap_;
	std::map<std::string, Glib::RefPtr<Gtk::Adjustment>> unitsToAdjMap_;

};

class HNumWidget : public NumWidget
{
public:
	HNumWidget(Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units = units_default);
	HNumWidget(const char *name, Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units = units_default);
	virtual ~HNumWidget() {}
};

class VNumWidget : public NumWidget
{
public:
	VNumWidget(Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units = units_default);
	VNumWidget(const char *name, Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units = units_default);
	virtual ~VNumWidget() {}
};


} // namespace

#endif
