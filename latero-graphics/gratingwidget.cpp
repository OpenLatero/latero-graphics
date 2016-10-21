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

#include "gratingwidget.h"
#include "grating.h"
#include <gtkmm/spinbutton.h>
#include <gtkmm/table.h>
#include "gtk/numwidget.h"

namespace latero {
namespace graphics { 

// GratingInvertCtrl ///////////////////////////////////////////////////////////////

GratingInvertCtrl::GratingInvertCtrl(GratingPtr peer) :
	check_("invert"),
	peer_(peer)
{
	add(check_);
	check_.set_active(peer_->GetInvert());
	check_.signal_clicked().connect(sigc::mem_fun(*this, &GratingInvertCtrl::OnClick));
}
void GratingInvertCtrl::OnClick() { peer_->SetInvert(check_.get_active()); };


// GratingEnableCtrl ///////////////////////////////////////////////////////////////

GratingEnableCtrl::GratingEnableCtrl(GratingPtr peer) :
	check_("enable"),
	peer_(peer)
{
	add(check_);
	check_.set_active(peer_->GetEnable());
	check_.signal_clicked().connect(
		sigc::mem_fun(*this, &GratingEnableCtrl::OnClick));
}

void GratingEnableCtrl::OnClick()
{
	peer_->SetEnable(check_.get_active());
	signalClick_();
};


// GratingRegularizeButton ///////////////////////////////////////////////////////////////

GratingRegularizeButton::GratingRegularizeButton(GratingPtr peer) : Gtk::Button("regularize"), peer_(peer) {}
void GratingRegularizeButton::on_clicked() { peer_->Regularize(); Gtk::Button::on_clicked(); };

// GratingRhythmWidget ////////////////////////////////////////////////////////////////////

GratingRhythmWidget::GratingRhythmWidget(GratingPtr peer) : 
    Gtk::Frame("Rhythm"), peer_(peer)
{
	set_size_request(-1,20);
	std::vector<double> rhythm = peer->GetRhythm();
	for (unsigned int i=0; i<rhythm.size(); ++i)
		AppendCycle(rhythm[i]*100);

	Gtk::Button *plusButton = manage(new Gtk::Button("+"));
	plusButton->signal_clicked().connect(
		sigc::mem_fun(*this, &GratingRhythmWidget::OnAddCycle));
	buttonBox_.pack_start(*plusButton,Gtk::PACK_SHRINK);

	Gtk::Button *minusButton = manage(new Gtk::Button("-"));
	minusButton->signal_clicked().connect(
		sigc::mem_fun(*this, &GratingRhythmWidget::OnRemoveCycle));
	buttonBox_.pack_start(*minusButton,Gtk::PACK_SHRINK);

	Gtk::HBox *box = manage(new Gtk::HBox);
	add(*box);
	box->pack_start(sliderBox_,Gtk::PACK_SHRINK);
	box->pack_start(buttonBox_,Gtk::PACK_SHRINK);
}

GratingRhythmWidget::~GratingRhythmWidget()
{
	while(adj_.size())
		RemoveCycle();
}

void GratingRhythmWidget::OnChanged()
{
	std::vector<double> rhythm;
	for (unsigned int i=0; i<adj_.size(); ++i)
		rhythm.push_back(adj_[i]->get_value()/100);
	peer_->SetRhythm(rhythm);
}

void GratingRhythmWidget::RemoveCycle()
{
	Gtk::Widget *widget = widget_[widget_.size()-1];
	widget_.pop_back();
	sliderBox_.remove(*widget);
	delete widget;

	Gtk::Adjustment *adj = adj_[adj_.size()-1];
	adj_.pop_back();
	delete adj;
}

void GratingRhythmWidget::AppendCycle(double value)
{
	Gtk::Adjustment* adj = new Gtk::Adjustment(value, 0, 100, 10);
	adj_.push_back(adj);
	adj->signal_value_changed().connect(sigc::mem_fun(*this, &GratingRhythmWidget::OnChanged));

	Gtk::Widget *widget = new latero::graphics::gtk::VNumWidget(*adj,0);
	widget_.push_back(widget);
	sliderBox_.pack_start(*widget, Gtk::PACK_SHRINK);
	show_all_children();
}

void GratingRhythmWidget::OnAddCycle()
{
	AppendCycle(100);
	OnChanged();
}

void GratingRhythmWidget::OnRemoveCycle()
{
	RemoveCycle();
	OnChanged();
}


// GratingGapSizeWidget ////////////////////////////////////////////////////////////////////

GratingGapSizeWidget::GratingGapSizeWidget(GratingPtr peer) : 
    adj_(peer->GetGapSize(),0,100), peer_(peer)
{
	add(*manage(new gtk::HNumWidget("gap size", adj_, 2, peer->GetUnits())));
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &GratingGapSizeWidget::OnChanged));
}
void GratingGapSizeWidget::OnChanged() { peer_->SetGapSize(adj_.get_value()); }


// GratingRidgeSizeWidget ////////////////////////////////////////////////////////////////////

GratingRidgeSizeWidget::GratingRidgeSizeWidget(GratingPtr peer) : 
	adj_(peer->GetRidgeSize(),0,100), peer_(peer)
{
	add(*manage(new gtk::HNumWidget("ridge size", adj_, 2, peer->GetUnits())));
	adj_.signal_value_changed().connect(
		sigc::mem_fun(*this, &GratingRidgeSizeWidget::OnChanged));
}
void GratingRidgeSizeWidget::OnChanged() { peer_->SetRidgeSize(adj_.get_value()); }


// GratingVelocityWidget ////////////////////////////////////////////////////////////////////

GratingVelocityWidget::GratingVelocityWidget(GratingPtr peer) : 
	distAdj_(peer->GetVel(),-100,100), hzAdj_(peer->GetVel(),-10,10), peer_(peer)
{
	gtk::HNumWidget *widget = new gtk::HNumWidget("velocity", distAdj_, 0, peer->GetUnits() + units::per_sec);
	widget->AddUnits(units::hz,&hzAdj_,2);
	widget->SelectUnits(peer->GetVelUnits());
	pack_start(*manage(widget));
	distAdj_.signal_value_changed().connect(sigc::mem_fun(*this, &GratingVelocityWidget::OnChangedDist));
	hzAdj_.signal_value_changed().connect(sigc::mem_fun(*this, &GratingVelocityWidget::OnChangedHz));
	widget->SignalUnitsChanged().connect(sigc::mem_fun(*this, &GratingVelocityWidget::OnChangedUnits));
}

void GratingVelocityWidget::OnChangedDist() 	{ peer_->SetVel(distAdj_.get_value()); }
void GratingVelocityWidget::OnChangedHz() 	{ peer_->SetVel(hzAdj_.get_value()); }

void GratingVelocityWidget::OnChangedUnits(std::string units)
{
	// TODO: causes a spurious change to peer when slider is updated
	peer_->SetVelUnitsHz(units==units::hz);
	double vel = peer_->GetVel();
	(units==units::hz) ? hzAdj_.set_value(vel) : distAdj_.set_value(vel);
}


// GratingCenterWidget ////////////////////////////////////////////////////////////////////

GratingCenterWidget::GratingCenterWidget(GratingPtr peer) : 
	adj_(100*peer->GetCenter(),100*Grating::center_min,100*Grating::center_max), peer_(peer)
{
	add(*manage(new gtk::HNumWidget("center", adj_, 0, "%")));	adj_.signal_value_changed().connect(
		sigc::mem_fun(*this, &GratingCenterWidget::OnChanged));
}

void GratingCenterWidget::OnChanged()
{
	peer_->SetCenter(adj_.get_value()/100);
}


// GratingInterpWidget ////////////////////////////////////////////////////////////////////

GratingInterpWidget::GratingInterpWidget(GratingPtr peer) : 
	Gtk::Frame("interpolation"), linearRadio_("linear"), arcRadio_("arc"), peer_(peer)
{
	Gtk::RadioButton::Group group = linearRadio_.get_group();
	arcRadio_.set_group(group);

	Grating::InterpMode mode = peer_->GetInterp();
	if (mode == Grating::interp_linear)
		linearRadio_.set_active();
	else // if (mode == Grating::interp_arc)
		arcRadio_.set_active();
	
	Gtk::HBox *box = manage(new Gtk::HBox);
	add(*box);
	box->pack_start(linearRadio_);
	box->pack_start(arcRadio_);
	linearRadio_.signal_clicked().connect(
		sigc::mem_fun(*this, &GratingInterpWidget::OnChanged));
	//arcRadio_.signal_clicked().connect(
	//	sigc::mem_fun(*this, &GratingInterpWidget::OnChanged));
}

void GratingInterpWidget::OnChanged()
{
	if (linearRadio_.get_active())
		peer_->SetInterp(Grating::interp_linear);
	else // if (arcRadio_.get_active())
		peer_->SetInterp(Grating::interp_arc);
}

// GratingCycleWidget ////////////////////////////////////////////////////////////////////

GratingCycleWidget::GratingCycleWidget(GratingPtr peer) :
	interpWidget_(peer), centerWidget_(peer), graph_(peer), peer_(peer)
{
	Gtk::VBox *box = manage(new Gtk::VBox);
	box->pack_start(interpWidget_);
	box->pack_start(centerWidget_);
	pack_start(*box);
	pack_start(graph_);
	graph_.Refresh();
	show_all_children();
}

// GratingWidgetSet ////////////////////////////////////////////////////////////////////

GratingWidgetSet::GratingWidgetSet(GratingPtr peer) :
	enableCtrl_(peer),
	velocityWidget_(peer),
	pitchWidget_(peer),
	advButton_(peer),
	invertCtrl_(peer),
	compressCtrl_(peer->GetModulator()), 
    peer_(peer)
{
	enableCtrl_.SignalClicked().connect(sigc::mem_fun(*this, &GratingWidgetSet::OnEnable));
};


void GratingWidgetSet::OnEnable()
{
	bool en = peer_->GetEnable();
	pitchWidget_.set_sensitive(en);
	velocityWidget_.set_sensitive(en);
	advButton_.set_sensitive(en);
	invertCtrl_.set_sensitive(en);
	compressCtrl_.set_sensitive(en);
}

// GratingAdvancedButton ////////////////////////////////////////////////////////////////////

GratingAdvancedButton::GratingAdvancedButton(GratingPtr peer) :
	Gtk::Button("more options"), dialog_(peer), peer_(peer)
{
	signal_released().connect(sigc::mem_fun(*this, &GratingAdvancedButton::OnPush));
};
void GratingAdvancedButton::OnPush() { dialog_.show(); }

// GratingWidget ////////////////////////////////////////////////////////////////////

GratingPitchWidget::GratingPitchWidget(GratingPtr peer) :
	ridgeWidget_(NULL), gapWidget_(NULL), regularizeButton_(peer), peer_(peer)
{
	ridgeWidget_ = manage(new GratingRidgeSizeWidget(peer));
	gapWidget_ = manage(new GratingGapSizeWidget(peer));

	pack_start(*ridgeWidget_);
	pack_start(*gapWidget_);
	if (peer->GetRegularizable()) pack_start(regularizeButton_,Gtk::PACK_SHRINK);
	regularizeButton_.signal_clicked().connect(sigc::mem_fun(*this, &GratingPitchWidget::OnRegularize));
}

void GratingPitchWidget::OnRegularize()
{
	printf("OnRegularized\n");

	remove(*ridgeWidget_);
	remove(*gapWidget_);
	remove(regularizeButton_);

	delete ridgeWidget_;
	delete gapWidget_;

	ridgeWidget_ = manage(new GratingRidgeSizeWidget(peer_));
	gapWidget_ = manage(new GratingGapSizeWidget(peer_));

	pack_start(*ridgeWidget_);
	pack_start(*gapWidget_);
	if (peer_->GetRegularizable()) pack_start(regularizeButton_,Gtk::PACK_SHRINK);
	show_all_children();
}


GratingWidget::GratingWidget(GratingPtr peer) :
	gtk::CheckFrame(peer->GetEnable(), "grating"), peer_(peer)
{
	Gtk::Table *table = manage(new Gtk::Table(2,3));
	GetBox().add(*table);
	table->attach(*manage(new GratingPitchWidget(peer)), 	0,2, 0,1);
	table->attach(*manage(new GratingVelocityWidget(peer)), 0,2, 1,2);
	table->attach(*manage(new GratingAdvancedButton(peer)), 0,1, 2,3);
	table->attach(*manage(new GratingInvertCtrl(peer)), 	1,2, 2,3);
	GetCheck().signal_clicked().connect(sigc::mem_fun(*this, &GratingWidget::OnEnable));
};
void GratingWidget::OnEnable() { peer_->SetEnable(GetCheck().get_active()); }

// AdvancedGratingWidget ////////////////////////////////////////////////////////////////////

AdvancedGratingWidget::AdvancedGratingWidget(GratingPtr peer) :
	Gtk::Dialog("Advanced Grating Options")
{
	Gtk::HBox *box = manage(new Gtk::HBox);
	get_vbox()->pack_start(*box);
	box->pack_start(*manage(new GratingRhythmWidget(peer)));
	box->pack_start(*manage(new GratingCycleWidget(peer)));
	box->pack_start(*manage(new GratingModulatorCtrl(peer->GetModulator())));
	show_all_children();
}

// GratingAmplitudeWidget ////////////////////////////////////////////////////////////////////

GratingAmplitudeWidget::GratingAmplitudeWidget(GratingPtr peer) : 
    adj_(peer->GetAmplitude()*100,0,100), peer_(peer)
{
	add(*manage(new gtk::HNumWidget("amplitude", adj_, 0, units::percent)));
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &GratingAmplitudeWidget::OnChanged));
}
void GratingAmplitudeWidget::OnChanged() { peer_->SetAmplitude(adj_.get_value()/100); }


} // namespace graphics
} // namespace latero
