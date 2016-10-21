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

#include "velmodulatorwidget.h"
#include "velmodulator.h"
#include "modulatorpreview.h"
#include <gtkmm.h>
#include "../../gtk/numwidget.h"

namespace latero {
namespace graphics { 

namespace vel_modulator_ctrls {

class Ctrl : public Gtk::VBox
{
public:
	Ctrl(VelModulatorPtr peer) : peer_(peer) {}
	virtual ~Ctrl() {}
protected:
	VelModulatorPtr peer_;
};

class LimitCtrl : public Ctrl
{
public:
	LimitCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(peer->GetLimit(), 0, 100)
	{
		add(*manage(new gtk::HNumWidget("lower limit", adj_, 0, units::mm_per_sec)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &LimitCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetLimit(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

class TransitionCtrl : public Ctrl
{
public:
	TransitionCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(peer->GetTransition(), 0, 100)
	{
		add(*manage(new gtk::HNumWidget("transition rate", adj_, 0, units::mm_per_sec)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &TransitionCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetTransition(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

class DelayCtrl : public Ctrl
{
public:
	DelayCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(peer->GetDelay(), 0, 2000)
	{
		add(*manage(new gtk::HNumWidget("delayed onset", adj_, 0, units::ms)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &DelayCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDelay(adj_.get_value()); }
	Gtk::Adjustment adj_;
};


} // namespace vel_modulator_ctrls


VelModulatorWidget::VelModulatorWidget(VelModulatorPtr peer) :
	peer_(peer)
{
	using namespace vel_modulator_ctrls;
	Gtk::VBox *box = manage(new Gtk::VBox);
	box->pack_start(*manage(new LimitCtrl(peer)));
	box->pack_start(*manage(new TransitionCtrl(peer)));
	box->pack_start(*manage(new DelayCtrl(peer)));
	pack_start(*box);
	pack_start(*manage(new ModulatorPreview(peer)), Gtk::PACK_SHRINK);
	show_all_children();
}

} // namespace graphics
} // namespace latero
