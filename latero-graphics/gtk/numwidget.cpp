#include "numwidget.h"
#include <iostream>

namespace latero::graphics::gtk {

NumWidget::NumWidget(Gtk::Orientation orient, Glib::RefPtr<Gtk::Adjustment> adj, uint digits, std::string units, std::string name) :
	units_(units),
	spin_(adj)
{
	if (name != "") set_label(name);

	auto mainBox = Gtk::make_managed<Gtk::Box>(orient);
	set_child(*mainBox);

	if (orient == Gtk::Orientation::VERTICAL)
	{
		scale_ = Gtk::make_managed<Gtk::Scale>(adj, Gtk::Orientation::VERTICAL);
		scale_->set_inverted();
		unitsDropDownBox_ = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	}
	else
	{
		scale_ = Gtk::make_managed<Gtk::Scale>(adj, Gtk::Orientation::HORIZONTAL);
		unitsDropDownBox_ = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	}

	SetDigits(digits);
    
	// This policy caused the update to occur only when then slider was released. It was disabled since it is no 
	// longer available in GTKMM3. This may cause expensive updates to occur while the slider is being dragged,
	// in which case the policy should be emulated by not handling all updates the same.
    //scale_->set_update_policy(Gtk::UPDATE_DISCONTINUOUS);

	if (orient == Gtk::Orientation::VERTICAL)
	{
		mainBox->append(*scale_);
		mainBox->append(spin_);
		mainBox->append(*unitsDropDownBox_);
		scale_->set_size_request(-1, 150);
	}
	else
	{
		mainBox->append(spin_);
		mainBox->append(*unitsDropDownBox_);
		mainBox->append(*scale_);
		scale_->set_size_request(150, -1);
	}
	scale_->set_hexpand();
	scale_->set_vexpand();

	scale_->set_format_value_func(sigc::mem_fun(*this, &NumWidget::OnFormat));

	unitsStringList_ = Gtk::StringList::create({});
	unitsDropDown_ = Gtk::make_managed<Gtk::DropDown>(unitsStringList_);
	unitsStringList_->append(units);
	unitsToDigitsMap_[units] = digits;
	unitsToAdjMap_[units] = adj;
	SelectUnits(units);
	unitsDropDown_->property_selected().signal_changed().connect(sigc::mem_fun(*this, &NumWidget::OnUnitsChanged));
};

void NumWidget::AddUnits(std::string units, Glib::RefPtr<Gtk::Adjustment> adj, uint digits)
{
	unitsStringList_->append(units);
	unitsToDigitsMap_[units] = digits;
	unitsToAdjMap_[units] = adj;
	if (unitsStringList_->get_n_items() > 1)
		unitsDropDownBox_->append(*unitsDropDown_);
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
	auto idx = unitsDropDown_->get_selected();
	if (idx == GTK_INVALID_LIST_POSITION) return;
	units_ = std::string(unitsStringList_->get_string(idx));
	SetDigits(10);
	SetAdjustment(unitsToAdjMap_[units_]);
	SetDigits(unitsToDigitsMap_[units_]);
	SignalUnitsChanged()(units_);
}

void NumWidget::SelectUnits(std::string units)
{
	for (guint i = 0; i < unitsStringList_->get_n_items(); ++i)
	{
		if (unitsStringList_->get_string(i) == units)
		{
			unitsDropDown_->set_selected(i);
			return;
		}
	}
}

Glib::ustring NumWidget::OnFormat(double v)
{
	std::stringstream stm;
	stm.setf(std::ios::fixed,std::ios::floatfield);
	stm.precision(scale_->get_digits());
	stm << v;
	if (units_ != units::none)
	{
		if ((units_ != units::percent)&&(units_ != units::degree)&&(units_ != units::degree_per_sec))
			stm << " ";
		stm << units_;
	}
	return Glib::ustring(stm.str().c_str());
}

} // namespace


