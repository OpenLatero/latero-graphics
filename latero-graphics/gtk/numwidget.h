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

#include <gtkmm/frame.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/entry.h>
#include <gtkmm/scale.h>
#include <gtkmm/box.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/alignment.h>
#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>
#include <assert.h>
#include <sstream>
#include "../units.h"

namespace latero {
namespace graphics { 

namespace gtk {

typedef enum {ORIENT_H, ORIENT_V} orient_T;

static const std::string units_default = "";
static const std::string name_default = "";

class NumWidgetCombo : public Gtk::ComboBox
{
public:
	NumWidgetCombo();
	virtual ~NumWidgetCombo() {};

	void Append(std::string units, Gtk::Adjustment* adj, uint digits);
	void SetActive(std::string units);
	int GetSize();

	std::string GetUnits();
	Gtk::Adjustment* GetAdj();
	uint GetDigits();

	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns() { add(units); add(adj); add(digits);}
		Gtk::TreeModelColumn<std::string> units;
		Gtk::TreeModelColumn<Gtk::Adjustment*> adj;
		Gtk::TreeModelColumn<uint> digits;
	};
	ModelColumns columns_;
	Glib::RefPtr<Gtk::ListStore> model_;
};

class NumWidget : public Gtk::Frame
{
public:
	NumWidget(orient_T orient, Gtk::Adjustment &adj, uint digits, std::string name = name_default, std::string units = units_default);
	virtual ~NumWidget() {}
	void AddUnits(std::string units, Gtk::Adjustment* adj, uint digits);
	void SelectUnits(std::string units);

	sigc::signal<void,std::string> SignalUnitsChanged() { return signalUnitsChanged_; }
protected:
	sigc::signal<void,std::string> signalUnitsChanged_;
	void SetDigits(uint n);
	void SetAdjustment(Gtk::Adjustment*adj);
	Glib::ustring OnFormat(double v);
	void OnUnitsChanged();

 	std::string units_;
	NumWidgetCombo unitsCombo_;
	Gtk::SpinButton spin_;
	Gtk::Box *box_, *comboBox_;
	Gtk::Scale *scale_;
};

class HNumWidget : public NumWidget
{
public:
	HNumWidget(Gtk::Adjustment &adj, uint digits, std::string units = units_default);
	HNumWidget(const char *name, Gtk::Adjustment &adj, uint digits, std::string units = units_default);
	virtual ~HNumWidget() {}
};

class VNumWidget : public NumWidget
{
public:
	VNumWidget(Gtk::Adjustment &adj, uint digits, std::string units = units_default);
	VNumWidget(const char *name, Gtk::Adjustment &adj, uint digits, std::string units = units_default);
	virtual ~VNumWidget() {}
};


} // namespace gtk

} // namespace graphics
} // namespace latero

#endif
