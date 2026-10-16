#pragma once

#include <gtkmm.h>
#include <assert.h>
#include <sstream>
#include "../units.h"

namespace latero::graphics::gtk {

class NumWidget : public Gtk::Frame
{
public:
	NumWidget(Gtk::Orientation orient, Glib::RefPtr<Gtk::Adjustment> adj, unsigned int digits, std::string units, std::string name = "");
	virtual ~NumWidget() {}
	void AddUnits(std::string units, Glib::RefPtr<Gtk::Adjustment> adj, unsigned int digits);
	void SelectUnits(std::string units);

	sigc::signal<void(std::string)> SignalUnitsChanged() { return signalUnitsChanged_; }
protected:
	sigc::signal<void(std::string)> signalUnitsChanged_;
	void SetDigits(unsigned int n);
	void SetAdjustment(Glib::RefPtr<Gtk::Adjustment> adj);
	Glib::ustring OnFormat(double v);
	void OnUnitsChanged();

	Gtk::Box *unitsDropDownBox_;
	Gtk::DropDown *unitsDropDown_;
	Glib::RefPtr<Gtk::StringList> unitsStringList_;
	Gtk::SpinButton spin_;
	Gtk::Scale *scale_;

	std::string units_;
	std::map<std::string, unsigned int> unitsToDigitsMap_;
	std::map<std::string, Glib::RefPtr<Gtk::Adjustment>> unitsToAdjMap_;

};

} // namespace

