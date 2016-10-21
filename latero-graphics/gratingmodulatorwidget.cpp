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
			append_text(ops[i].label);
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


class GratingModulatorFactorWidget : public Gtk::HBox
{
public:
	GratingModulatorFactorWidget(GratingModulatorPtr peer) : 
		adj_(peer->GetFactor(),0.1,10), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget(adj_, 1)));
		adj_.signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorFactorWidget::OnChanged));
	}
	virtual ~GratingModulatorFactorWidget() {}
protected:
	void OnChanged() { peer_->SetFactor(adj_.get_value()); }
	Gtk::Adjustment adj_;
	GratingModulatorPtr peer_;
};

class GratingModulatorLengthWidget : public Gtk::HBox
{
public:
	GratingModulatorLengthWidget(GratingModulatorPtr peer) : 
		adj_(peer->GetLength(),1,300), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget(adj_,0,peer->GetUnits())));
		adj_.signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorLengthWidget::OnChanged));
	}
	virtual ~GratingModulatorLengthWidget() {}
protected:
	void OnChanged() { peer_->SetLength(adj_.get_value()); }
	Gtk::Adjustment adj_;
	GratingModulatorPtr peer_;
};

class GratingModulatorPosWidget : public Gtk::HBox
{
public:
	GratingModulatorPosWidget(GratingModulatorPtr peer) : 
		adj_(peer->GetPosition(),0,360), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget(adj_,0,peer->GetUnits())));
		adj_.signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorPosWidget::OnChanged));
	}
	virtual ~GratingModulatorPosWidget() {}
protected:
	void OnChanged() { peer_->SetPosition(adj_.get_value()); }
	Gtk::Adjustment adj_;
	GratingModulatorPtr peer_;
};


GratingModulatorCtrl::GratingModulatorCtrl(GratingModulatorPtr peer) :
	Gtk::Frame("pitch modulation"), peer_(peer)
{
	Gtk::HBox *box = manage(new Gtk::HBox);
	
	add(*box);
	box->pack_start(check_, Gtk::PACK_SHRINK);
	box->pack_start(widgetbox_);
	widgetbox_.pack_start(*manage(new GratingModulatorModeCombo(peer)), Gtk::PACK_SHRINK);
	widgetbox_.pack_start(*manage(new GratingModulatorFactorWidget(peer)));
	widgetbox_.pack_start(*manage(new GratingModulatorPosWidget(peer)));
	widgetbox_.pack_start(*manage(new GratingModulatorLengthWidget(peer)));

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
