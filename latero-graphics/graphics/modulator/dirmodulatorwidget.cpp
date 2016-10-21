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

#include "dirmodulatorwidget.h"
#include "dirmodulator.h"
#include "modulatorpreview.h"
#include <gtkmm.h>
#include "../../gtk/numwidget.h"

namespace latero {
namespace graphics { 

namespace dir_modulator_ctrls {

class Ctrl : public Gtk::VBox
{
public:
	Ctrl(DirModulatorPtr peer) : peer_(peer) {}
	virtual ~Ctrl() {}
protected:
	DirModulatorPtr peer_;
};

class LowVelModeCombo : public Ctrl
{
public:
	LowVelModeCombo(DirModulatorPtr peer) : Ctrl(peer)
	{
		Gtk::Frame *frame = manage(new Gtk::Frame("low velocity mode"));
		add(*frame);
		frame->add(combo_);
		std::vector<std::string> labels = peer->GetLowVelModeLabels();
		for (unsigned int i=0; i<labels.size(); ++i)
			combo_.append_text(labels[i]);
		combo_.set_active_text(peer->GetLowVelModeLabel());
		combo_.signal_changed().connect(sigc::mem_fun(*this, &LowVelModeCombo::OnChange));
	}
	virtual ~LowVelModeCombo() {};
	sigc::signal<void> SignalChanged() { return signalChanged_; };
private:
	sigc::signal<void> signalChanged_;
	void OnChange() { peer_->SetLowVelModeLabel(combo_.get_active_text()); signalChanged_(); }
	Gtk::ComboBoxText combo_;
};


class DirCtrl : public Ctrl
{
public:
	DirCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(peer->GetDirection(), 0, 360)
	{
		add(*manage(new gtk::HNumWidget("direction", adj_, 0, units::degree)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &DirCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDirection(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

class DirToleranceCtrl : public Ctrl
{
public:
	DirToleranceCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(peer->GetDirTolerance(), 0, 180)
	{
		add(*manage(new gtk::HNumWidget("direction tolerance", adj_, 0, units::degree)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &DirToleranceCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDirTolerance(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

class DirTransitionCtrl : public Ctrl
{
public:
	DirTransitionCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(peer->GetDirTransition(), 1, 180)
	{
		add(*manage(new gtk::HNumWidget("direction transition rate", adj_, 0, units::degree)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &DirTransitionCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDirTransition(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

class VelToleranceCtrl : public Ctrl
{
public:
	VelToleranceCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(peer->GetVelTolerance(), 0, 100)
	{
		add(*manage(new gtk::HNumWidget("velocity tolerance", adj_, 0, units::mm_per_sec)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &VelToleranceCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetVelTolerance(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

class VelTransitionCtrl : public Ctrl
{
public:
	VelTransitionCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(peer->GetVelTransition(), 0, 100)
	{
		add(*manage(new gtk::HNumWidget("velocity transition rate", adj_, 0, units::mm_per_sec)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &VelTransitionCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetVelTransition(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

class SymmetricCtrl : public Ctrl
{
public:
	SymmetricCtrl(DirModulatorPtr peer) : Ctrl(peer), check_("symmetric")
	{
		Gtk::Frame *frame = manage(new Gtk::Frame);
		add(*frame);
		frame->add(check_);
		check_.set_active(peer_->GetSymmetric());
		check_.signal_clicked().connect(sigc::mem_fun(*this, &SymmetricCtrl::OnClick));
	}
	void OnClick() { peer_->SetSymmetric(check_.get_active()); };
protected:
	Gtk::CheckButton check_;
};

} // namespace


DirModulatorWidget::DirModulatorWidget(DirModulatorPtr peer) :
	peer_(peer)
{
	using namespace dir_modulator_ctrls;
	Gtk::Table *table = manage(new Gtk::Table(3,3));
	table->attach(*manage(new DirCtrl(peer)),0,1,0,1);
	table->attach(*manage(new SymmetricCtrl(peer)),1,2,0,1);
	table->attach(*manage(new LowVelModeCombo(peer)),2,3,0,1);
	table->attach(*manage(new DirToleranceCtrl(peer)),0,1,1,2);
	table->attach(*manage(new DirTransitionCtrl(peer)),1,3,1,2);
	table->attach(*manage(new VelToleranceCtrl(peer)),0,1,2,3);
	table->attach(*manage(new VelTransitionCtrl(peer)),1,3,2,3);
	pack_start(*table);
	pack_start(*manage(new ModulatorPreview(peer)), Gtk::PACK_SHRINK);
	show_all_children();
}

} // namespace graphics
} // namespace latero
