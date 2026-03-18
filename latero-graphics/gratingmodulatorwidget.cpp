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

#include "gratingmodulatorwidget.h"
#include "gratingmodulator.h"
#include "gtk/numwidget.h"

namespace latero {
namespace graphics { 

class GratingModulatorModeCombo : public Gtk::ComboBoxText
{
public:
	GratingModulatorModeCombo(GratingModulatorPtr peer) :
		peer_(peer)
	{
		GratingModulator::ModeSet ops = peer->GetModes();
		for (unsigned int i=0; i<ops.size(); ++i)
			append(ops[i].label);
		set_active_text(peer->GetMode().label);
		signal_changed().connect(sigc::mem_fun(*this, &GratingModulatorModeCombo::OnChange));
	}
	virtual ~GratingModulatorModeCombo() {};
	sigc::signal<void> SignalChanged() { return signalChanged_; };
private:
	sigc::signal<void> signalChanged_;
	void OnChange()  { peer_->SetMode(get_active_text()); signalChanged_(); };
	GratingModulatorPtr peer_;
};


class GratingModulatorFactorWidget : public Gtk::Box
{
public:
	GratingModulatorFactorWidget(GratingModulatorPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetFactor(),0.1,10)), peer_(peer)
	{
		add(*Gtk::manage(new gtk::HNumWidget(adj_, 1)));
		adj_->signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorFactorWidget::OnChanged));
	}
	virtual ~GratingModulatorFactorWidget() {}
protected:
	void OnChanged() { peer_->SetFactor(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	GratingModulatorPtr peer_;
};

class GratingModulatorLengthWidget : public Gtk::Box
{
public:
	GratingModulatorLengthWidget(GratingModulatorPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetLength(),1,300)), peer_(peer)
	{
		add(*Gtk::manage(new gtk::HNumWidget(adj_,0,peer->GetUnits())));
		adj_->signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorLengthWidget::OnChanged));
	}
	virtual ~GratingModulatorLengthWidget() {}
protected:
	void OnChanged() { peer_->SetLength(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	GratingModulatorPtr peer_;
};

class GratingModulatorPosWidget : public Gtk::Box
{
public:
	GratingModulatorPosWidget(GratingModulatorPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetPosition(),0,360)), peer_(peer)
	{
		add(*Gtk::manage(new gtk::HNumWidget(adj_,0,peer->GetUnits())));
		adj_->signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorPosWidget::OnChanged));
	}
	virtual ~GratingModulatorPosWidget() {}
protected:
	void OnChanged() { peer_->SetPosition(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	GratingModulatorPtr peer_;
};


GratingModulatorCtrl::GratingModulatorCtrl(GratingModulatorPtr peer) :
	widgetbox_(Gtk::Orientation::HORIZONTAL), Gtk::Frame("pitch modulation"), peer_(peer)
{
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	
	auto modeCombo = Gtk::manage(new GratingModulatorModeCombo(peer));
	auto factorWidget = Gtk::manage(new GratingModulatorFactorWidget(peer);
	auto posWidget = Gtk::manage(new GratingModulatorPosWidget(peer));
	auto lengthWidget = Gtk::manage(new GratingModulatorLengthWidget(peer));

	widgetbox_->set_hexpand();
	factorWidget->set_hexpand();
	posWidget->set_hexpand();
	lengthWidget->set_hexpand();

	add(*box);
	box->append(check_);
	box->append(widgetbox_);
	widgetbox_.append(*modeCombo);
	widgetbox_.append(*factorWidget));
	widgetbox_.append(*posWidget);
	widgetbox_.append(*lengthWidget);

	check_.set_active(peer_->GetEnable());
	widgetbox_.set_sensitive(check_.get_active());
	check_.signal_clicked().connect(sigc::mem_fun(*this, &GratingModulatorCtrl::OnClick));
}
    
void GratingModulatorCtrl::OnClick()
{
	peer_->SetEnable(check_.get_active());
	widgetbox_.set_sensitive(check_.get_active());
};

} // namespace graphics
} // namespace latero
