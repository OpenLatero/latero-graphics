#pragma once

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

