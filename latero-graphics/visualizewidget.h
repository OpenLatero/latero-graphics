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

#ifndef LATERO_GRAPHICS_VISUALIZE_WIDGET_H
#define LATERO_GRAPHICS_VISUALIZE_WIDGET_H

#include <gtkmm.h>
#include "positiongenfwd.h"
#include "gtk/animation.h"
#include "units.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>


namespace latero {
namespace graphics {

class TimeWidget : public Gtk::Box
{
public:
	TimeWidget(int init=0, std::string units=units::nsec) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(init, 0, 1000000)), spin_(adj_)
	{
		unitsList_ = Gtk::StringList::create({});
		unitsList_->append(units::day);
		unitsList_->append(units::hour);
		unitsList_->append(units::min);
		unitsList_->append(units::sec);
		unitsList_->append(units::msec);
		unitsList_->append(units::usec);
		unitsList_->append(units::nsec);
		unitsDropDown_ = Gtk::make_managed<Gtk::DropDown>(unitsList_);
		for (guint i = 0; i < unitsList_->get_n_items(); ++i)
			if (unitsList_->get_string(i) == units) { unitsDropDown_->set_selected(i); break; }

		append(spin_);
		spin_.set_hexpand();
		spin_.set_margin_end(6);
		append(*unitsDropDown_);
	};

	virtual ~TimeWidget() {};

	boost::posix_time::time_duration GetTime()
	{
		auto idx = unitsDropDown_->get_selected();
		return units::ConvertTime(adj_->get_value(), std::string(unitsList_->get_string(idx)));
	}

protected:

    Glib::RefPtr<Gtk::Adjustment> adj_;
	Gtk::SpinButton spin_;
	Glib::RefPtr<Gtk::StringList> unitsList_;
	Gtk::DropDown* unitsDropDown_;
};

class StartTimeWidget : public Gtk::Box
{
public:
	StartTimeWidget() : Gtk::Box(Gtk::Orientation::HORIZONTAL), currentRadio_("now"), timeCtrl_(0, units::min)
	{
		auto pTimeBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);

		currentRadio_.set_group(timeRadio_);
		timeRadio_.set_active();
	
		pTimeBox->append(timeRadio_);
		pTimeBox->append(timeCtrl_);
		timeCtrl_.set_hexpand();

		append(currentRadio_);		
		currentRadio_.set_margin_end(6);
		append(*pTimeBox);
		pTimeBox->set_hexpand();

		timeRadio_.signal_toggled().connect(sigc::mem_fun(*this, &StartTimeWidget::OnModeChanged));
	};

	virtual ~StartTimeWidget() {};

	void OnModeChanged() { timeCtrl_.set_sensitive(timeRadio_.get_active()); }

	boost::posix_time::time_duration GetTime()
	{
		// TODO: get this to work again?
		//if (currentRadio_.get_active())
		//	return rt_timer_read();
		//else
			return timeCtrl_.GetTime();
	}

	Gtk::CheckButton currentRadio_, timeRadio_;
	TimeWidget timeCtrl_;
};


/**
 * This widget takes care of showing a low-level visualization image
 * for PositionGen generator. In order to work properly, the PositionGen 
 * must not depend on time.
 */
class VisualizeWidget : public Gtk::Window
{
public:
	VisualizeWidget(PositionGenPtr gen);
	virtual ~VisualizeWidget();

protected:
	Gtk::Widget *GetAnimWidget();
	Gtk::Widget *GetPlaybackWidget();
	latero::graphics::gtk::Animation GetDeflectionMap(uint w, uint n, double velMag, double velDir);
	void RefreshImg();
	void CreateMenu();
	void OnSave();
	void OnSaveAs();
	virtual void on_realize();
	void ReloadAnimation();
	void OnApply();
	void OnPlay();
	void OnStop();
	void OnNext();
	void OnPrevious();
	void OnWidthChanged();
	void OnHeightChanged();
	
    Glib::RefPtr<Gtk::Adjustment> velMagAdj_;
    Glib::RefPtr<Gtk::Adjustment> velDirAdj_;
	TimeWidget intervalCtrl_;
    Glib::RefPtr<Gtk::Adjustment> frameAdj_;
    Glib::RefPtr<Gtk::Adjustment> nbFramesAdj_;
    Glib::RefPtr<Gtk::Adjustment> widthAdj_, heightAdj_;
	StartTimeWidget startTimeCtrl_;
	std::unique_ptr<Gtk::PopoverMenu> popupMenu_;
	Gtk::Image img_;
	latero::graphics::gtk::Animation map_;
	Glib::RefPtr<Gtk::StringList> modeList_;
	Gtk::DropDown* modeDropDown_;
	PositionGenPtr gen_;
};

} // namespace graphics
} // namespace latero

#endif
