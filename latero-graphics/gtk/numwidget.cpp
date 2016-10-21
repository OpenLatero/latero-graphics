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

namespace latero {
namespace graphics { 

namespace gtk {

NumWidgetCombo::NumWidgetCombo()
{
	model_ = Gtk::ListStore::create(columns_);
	set_model(model_);
	pack_start(columns_.units);
}

void NumWidgetCombo::Append(std::string units, Gtk::Adjustment* adj, uint digits)
{
	Gtk::TreeModel::Row row = *(model_->append());
	row[columns_.units] = units;
	row[columns_.adj] = adj;
	row[columns_.digits] = digits;
}

int NumWidgetCombo::GetSize()
{
	return get_model()->children().size();
}

void NumWidgetCombo::SetActive(std::string units)
{
	Gtk::TreeModel::Children::iterator iter;
	for (iter = model_->children().begin(); iter != model_->children().end(); iter++)
	{
		std::string curunits = (*iter)[columns_.units];
		if (curunits == units)
		{
			set_active(iter);
		}
	}
}

std::string NumWidgetCombo::GetUnits()
{
	return (*get_active())[columns_.units];
}

Gtk::Adjustment* NumWidgetCombo::GetAdj()
{
	return (*get_active())[columns_.adj];
}

uint NumWidgetCombo::GetDigits()
{
	return (*get_active())[columns_.digits];
}





NumWidget::NumWidget(orient_T orient, Gtk::Adjustment &adj, uint digits, std::string name, std::string units) :
	units_(units),
	spin_(adj)
{
	Gtk::Box *box2;
	if (orient == ORIENT_V)
	{
		scale_ = manage(new Gtk::VScale(adj));
		scale_->set_inverted();
		box2 = manage(new Gtk::HBox);
		box_ = manage(new Gtk::VBox);
		comboBox_ = manage(new Gtk::HBox);
	}
	else
	{
		scale_ = manage(new Gtk::HScale(adj));
		box2 = manage(new Gtk::VBox);
		box_ = manage(new Gtk::HBox);
		comboBox_ = manage(new Gtk::VBox);
	}

	if (name=="")
	{
		set_shadow_type(Gtk::SHADOW_NONE);
	}
	else
	{
		if (name!="") set_label(name);
	}

	SetDigits(digits);
	scale_->set_update_policy(Gtk::UPDATE_DISCONTINUOUS);

	add(*box2);
	box2->pack_start(*box_, Gtk::PACK_SHRINK);
	if (orient == ORIENT_V)
	{
		box_->pack_start(*scale_); 
		box_->pack_start(spin_, Gtk::PACK_SHRINK);
		box_->pack_start(*comboBox_, Gtk::PACK_SHRINK);
	}
	else
	{
		box_->pack_start(spin_, Gtk::PACK_SHRINK);
		box_->pack_start(*comboBox_, Gtk::PACK_SHRINK);
		box_->pack_start(*scale_); 
	}

	scale_->signal_format_value().connect(
		sigc::mem_fun(*this, &NumWidget::OnFormat));

	unitsCombo_.Append(units,&adj,digits);
	unitsCombo_.SetActive(units);
	unitsCombo_.signal_changed().connect( sigc::mem_fun(*this, &NumWidget::OnUnitsChanged) );
};

void NumWidget::AddUnits(std::string units, Gtk::Adjustment* adj, uint digits)
{
	unitsCombo_.Append(units,adj,digits);
	if (unitsCombo_.GetSize())
		comboBox_->pack_start(unitsCombo_, Gtk::PACK_SHRINK);
}

void NumWidget::SetDigits(uint n)
{
	scale_->set_digits(n);
	spin_.set_digits(n);
}

void NumWidget::SetAdjustment(Gtk::Adjustment* adj)
{
	spin_.set_adjustment(*adj);
	scale_->set_adjustment(*adj);
}

void NumWidget::OnUnitsChanged()
{
	units_ = unitsCombo_.GetUnits();
	SetDigits(10);
	SetAdjustment(unitsCombo_.GetAdj());
	SetDigits(unitsCombo_.GetDigits());
	SignalUnitsChanged()(units_);
}

void NumWidget::SelectUnits(std::string units)
{
	unitsCombo_.SetActive(units);
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


HNumWidget::HNumWidget(Gtk::Adjustment &adj, uint digits, std::string units) :
	NumWidget(ORIENT_H,adj,digits,"",units) {}

VNumWidget::VNumWidget(Gtk::Adjustment &adj, uint digits, std::string units) :
	NumWidget(ORIENT_V,adj,digits,"",units) {}

HNumWidget::HNumWidget(const char *name, Gtk::Adjustment &adj, uint digits, std::string units) :
	NumWidget(ORIENT_H,adj,digits,name,units) {}

VNumWidget::VNumWidget(const char *name, Gtk::Adjustment &adj, uint digits, std::string units) :
	NumWidget(ORIENT_V,adj,digits,name,units) {}

} // namespace gtk

} // namespace graphics
} // namespace latero


