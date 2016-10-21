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

#include <gtkmm/dialog.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/image.h>
#include "positiongenfwd.h"
#include "gtk/animation.h"
#include "units.h"
#include "gtk/spacer.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>


namespace latero {
namespace graphics { 

class TimeWidget : public Gtk::HBox
{
public:
	TimeWidget(int init=0, std::string units=units::nsec) :
		adj_(init, 0, 1000000), spin_(adj_)
	{
		combo_.append_text(units::day);
		combo_.append_text(units::hour);
		combo_.append_text(units::min);
		combo_.append_text(units::sec);
		combo_.append_text(units::msec);
		combo_.append_text(units::usec);
		combo_.append_text(units::nsec);
		combo_.set_active_text(units);

		pack_start(spin_);
		pack_start(*manage(new gtk::HSpacer(6)), Gtk::PACK_SHRINK);
		pack_start(combo_, Gtk::PACK_SHRINK);
	};

	virtual ~TimeWidget() {};

	boost::posix_time::time_duration GetTime()
	{
		return units::ConvertTime(adj_.get_value(), combo_.get_active_text());
	}

protected:

	Gtk::Adjustment adj_;
	Gtk::SpinButton spin_;
	Gtk::ComboBoxText combo_;
};

class StartTimeWidget : public Gtk::HBox
{
public:
	StartTimeWidget() : currentRadio_("now"), timeCtrl_(0, units::min)
	{
		Gtk::HBox *pTimeBox = manage(new Gtk::HBox);

		Gtk::RadioButton::Group group = currentRadio_.get_group();
		timeRadio_.set_group(group);
		timeRadio_.set_active();
	
		pTimeBox->pack_start(timeRadio_, Gtk::PACK_SHRINK);
		pTimeBox->pack_start(timeCtrl_);

		pack_start(currentRadio_, Gtk::PACK_SHRINK);		
		pack_start(*manage(new gtk::HSpacer(6)), Gtk::PACK_SHRINK);
		pack_start(*pTimeBox);

		timeRadio_.signal_clicked().connect(sigc::mem_fun(*this, &StartTimeWidget::OnModeChanged));
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

	Gtk::RadioButton currentRadio_, timeRadio_;
	TimeWidget timeCtrl_;
};


/**
 * This widget takes care of showing a low-level visualization image
 * for PositionGen generator. In order to work properly, the PositionGen 
 * must not depend on time.
 *
 * @TODO: rename? use Window instead of dialog?
 */
class VisualizeWidget : public Gtk::Dialog
{
public:
	VisualizeWidget(PositionGenPtr gen);
	virtual ~VisualizeWidget();

protected:
	Gtk::Widget *GetAnimWidget();
	Gtk::Widget *GetPlaybackWidget();
	latero::graphics::gtk::Animation GetDeflectionMap(uint w, uint n, double velMag, double velDir);
	void RefreshImg();
	bool OnClick(GdkEventButton* event);
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
	
	Gtk::Adjustment velMagAdj_;
	Gtk::Adjustment velDirAdj_;
	TimeWidget intervalCtrl_;
	Gtk::Adjustment frameAdj_;
	Gtk::Adjustment nbFramesAdj_;
	Gtk::Adjustment widthAdj_, heightAdj_;
	StartTimeWidget startTimeCtrl_;
	Glib::RefPtr<Gtk::UIManager> uiManager_;
	Gtk::Image img_;
	latero::graphics::gtk::Animation map_;
	Gtk::ComboBoxText modeCombo_;
	PositionGenPtr gen_;
};

} // namespace graphics
} // namespace latero

#endif
