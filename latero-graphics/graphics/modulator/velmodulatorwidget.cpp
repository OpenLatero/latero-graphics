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
#include "../../gtk/numwidget.h"

namespace latero {
namespace graphics { 

namespace vel_modulator_ctrls {

class Ctrl : public Gtk::Box
{
public:
	Ctrl(VelModulatorPtr peer) : Gtk::Box(Gtk::Orientation::VERTICAL), peer_(peer) {}
	virtual ~Ctrl() {}
protected:
	VelModulatorPtr peer_;
};

class LimitCtrl : public Ctrl
{
public:
	LimitCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetLimit(), 0, 100))
	{
		add(*Gtk::manage(new gtk::HNumWidget("lower limit", adj_, 0, units::mm_per_sec)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &LimitCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetLimit(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class TransitionCtrl : public Ctrl
{
public:
	TransitionCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetTransition(), 0, 100))
	{
		add(*Gtk::manage(new gtk::HNumWidget("transition rate", adj_, 0, units::mm_per_sec)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &TransitionCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetTransition(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class DelayCtrl : public Ctrl
{
public:
	DelayCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetDelay(), 0, 2000))
	{
		add(*Gtk::manage(new gtk::HNumWidget("delayed onset", adj_, 0, units::ms)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DelayCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDelay(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};


} // namespace vel_modulator_ctrls


VelModulatorWidget::VelModulatorWidget(VelModulatorPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	using namespace vel_modulator_ctrls;

	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
	auto limitCtrl = Gtk::manage(new LimitCtrl(peer));
	auto transitionCtrl = Gtk::manage(new TransitionCtrl(peer));
	auto delayCtrl = Gtk::manage(new DelayCtrl(peer));
	auto previewCtrl = Gtk::manage(new ModulatorPreview(peer));

	limitCtrl->set_vexpand();
	transitionCtrl->set_vexpand();
	delayCtrl->set_vexpand();
	box->set_hexpand();

	box->append(*limitCtrl);
	box->append(*transitionCtrl);
	box->append(*delayCtrl);

	append(*box);
	append(*previewCtrl);

	show_all_children();
}

} // namespace graphics
} // namespace latero
