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

#include "numwidget.h"
#include <iostream>

namespace latero {
namespace graphics { 
namespace gtk {

NumWidget::NumWidget(Gtk::Orientation orient, Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string name, std::string units) :
	units_(units),
	spin_(adj)
{
	Gtk::Box *box2;
	if (orient == Gtk::Orientation::VERTICAL)
	{
		scale_ = Gtk::make_managed<Gtk::Scale>(adj, Gtk::Orientation::VERTICAL);
		scale_->set_inverted();
		box2 = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
		box_ = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
		comboBox_ = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	}
	else
	{
		scale_ = Gtk::make_managed<Gtk::Scale>(adj, Gtk::Orientation::HORIZONTAL);
		box2 = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
		box_ = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
		comboBox_ = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	}

	if (name != "") set_label(name);

	SetDigits(digits);
    
	// This policy caused the update to occur only when then slider was released. It was disabled since it is no 
	// longer available in GTKMM3. This may cause expensive updates to occur while the slider is being dragged,
	// in which case the policy should be emulated by not handling all updates the same.
    //scale_->set_update_policy(Gtk::UPDATE_DISCONTINUOUS);

	set_child(*box2);
	box2->append(*box_);
	if (orient == Gtk::Orientation::VERTICAL)
	{
		box_->append(*scale_);
		box_->append(spin_);
		box_->append(*comboBox_);
		scale_->set_size_request(-1, 150);
	}
	else
	{
		box_->append(spin_);
		box_->append(*comboBox_);
		box_->append(*scale_);
		scale_->set_size_request(150, -1);
	}
	scale_->set_hexpand();
	scale_->set_vexpand();

	scale_->set_format_value_func(sigc::mem_fun(*this, &NumWidget::OnFormat));

	unitsCombo_.append(units);
	unitsToDigitsMap_[units] = digits;
	unitsToAdjMap_[units] = adj;
	unitsCombo_.set_active_text(units);
	unitsCombo_.signal_changed().connect( sigc::mem_fun(*this, &NumWidget::OnUnitsChanged) );
};

void NumWidget::AddUnits(std::string units, Glib::RefPtr<Gtk::Adjustment> adj, uint digits)
{
	unitsCombo_.append(units);
	unitsToDigitsMap_[units] = digits;
	unitsToAdjMap_[units] = adj;
	if (unitsCombo_.get_model()->children().size())
		comboBox_->append(unitsCombo_);
}

void NumWidget::SetDigits(uint n)
{
	scale_->set_digits(n);
	spin_.set_digits(n);
}

void NumWidget::SetAdjustment(Glib::RefPtr<Gtk::Adjustment> adj)
{
	spin_.set_adjustment(adj);
	scale_->set_adjustment(adj);
}

void NumWidget::OnUnitsChanged()
{
	units_ = unitsCombo_.get_active_text();
	SetDigits(10);
	SetAdjustment(unitsToAdjMap_[units_]);
	SetDigits(unitsToDigitsMap_[units_]);
	SignalUnitsChanged()(units_);
}

void NumWidget::SelectUnits(std::string units)
{
	unitsCombo_.set_active_text(units);
}

Glib::ustring NumWidget::OnFormat(double v)
{
	std::stringstream stm;
	stm.setf(std::ios::fixed,std::ios::floatfield);
	stm.precision(scale_->get_digits());
	stm << v;
	if (units_ != "")
	{
		if ((units_ != units::percent)&&(units_ != units::degree)&&(units_ != units::degree_per_sec))
			stm << " ";
		stm << units_;
	}
	return Glib::ustring(stm.str().c_str());
}


HNumWidget::HNumWidget(Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units) :
	NumWidget(Gtk::Orientation::HORIZONTAL,adj,digits,"",units) {}

VNumWidget::VNumWidget(Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units) :
	NumWidget(Gtk::Orientation::VERTICAL,adj,digits,"",units) {}

HNumWidget::HNumWidget(const char *name, Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units) :
	NumWidget(Gtk::Orientation::HORIZONTAL,adj,digits,name,units) {}

VNumWidget::VNumWidget(const char *name, Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units) :
	NumWidget(Gtk::Orientation::VERTICAL,adj,digits,name,units) {}

} // namespace gtk

} // namespace graphics
} // namespace latero


