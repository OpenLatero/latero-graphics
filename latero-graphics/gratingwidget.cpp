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
#include "gtk/numwidget.h"
#include <iostream>

namespace latero {
namespace graphics { 

// GratingInvertCtrl ///////////////////////////////////////////////////////////////

GratingInvertCtrl::GratingInvertCtrl(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	check_("invert"),
	peer_(peer)
{
	append(check_);
	check_.set_active(peer_->GetInvert());
	check_.signal_toggled().connect(sigc::mem_fun(*this, &GratingInvertCtrl::OnClick));
}
void GratingInvertCtrl::OnClick() { peer_->SetInvert(check_.get_active()); };


// GratingEnableCtrl ///////////////////////////////////////////////////////////////

GratingEnableCtrl::GratingEnableCtrl(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	check_("enable"),
	peer_(peer)
{
	append(check_);
	check_.set_active(peer_->GetEnable());
	check_.signal_toggled().connect(
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
    sliderBox_(Gtk::Orientation::HORIZONTAL), Frame("Rhythm"), peer_(peer), buttonBox_(Gtk::Orientation::VERTICAL)
{
	set_size_request(-1,20);
	std::vector<double> rhythm = peer->GetRhythm();
	for (unsigned int i=0; i<rhythm.size(); ++i)
		AppendCycle(rhythm[i]*100);

	auto plusButton = Gtk::manage(new Gtk::Button("+"));
	plusButton->signal_clicked().connect(
		sigc::mem_fun(*this, &GratingRhythmWidget::OnAddCycle));
	buttonBox_.append(*plusButton);

	auto minusButton = Gtk::manage(new Gtk::Button("-"));
	minusButton->signal_clicked().connect(
		sigc::mem_fun(*this, &GratingRhythmWidget::OnRemoveCycle));
	buttonBox_.append(*minusButton);

	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	set_child(*box);
	box->append(sliderBox_);
	box->append(buttonBox_);
}

GratingRhythmWidget::~GratingRhythmWidget()
{
	while(adjVec_.size())
		RemoveCycle();
}

void GratingRhythmWidget::OnChanged()
{
	std::vector<double> rhythm;
	for (unsigned int i=0; i<adjVec_.size(); ++i)
		rhythm.push_back(adjVec_[i]->get_value()/100);
	peer_->SetRhythm(rhythm);
}

void GratingRhythmWidget::RemoveCycle()
{
	if (adjVec_.size()<=0) return;

	Gtk::Widget *widget = widget_[widget_.size()-1];
	widget_.pop_back();
	sliderBox_.remove(*widget);
	delete widget;

	adjVec_.pop_back();
}

void GratingRhythmWidget::AppendCycle(double value)
{
    Glib::RefPtr<Gtk::Adjustment> adj = Gtk::Adjustment::create(value, 0, 100, 10);
	adjVec_.push_back(adj);
	adj->signal_value_changed().connect(sigc::mem_fun(*this, &GratingRhythmWidget::OnChanged));

	Gtk::Widget *widget = new latero::graphics::gtk::VNumWidget(adj,0);
	widget_.push_back(widget);
	sliderBox_.append(*widget);
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
	Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetGapSize(),0,100)), peer_(peer)
{
	append(*Gtk::manage(new gtk::HNumWidget("gap size", adj_, 2, peer->GetUnits())));
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &GratingGapSizeWidget::OnChanged));
}
void GratingGapSizeWidget::OnChanged() { peer_->SetGapSize(adj_->get_value()); }


// GratingRidgeSizeWidget ////////////////////////////////////////////////////////////////////

GratingRidgeSizeWidget::GratingRidgeSizeWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetRidgeSize(),0,100)), peer_(peer)
{
	append(*Gtk::manage(new gtk::HNumWidget("ridge size", adj_, 2, peer->GetUnits())));
	adj_->signal_value_changed().connect(
		sigc::mem_fun(*this, &GratingRidgeSizeWidget::OnChanged));
}
void GratingRidgeSizeWidget::OnChanged() { peer_->SetRidgeSize(adj_->get_value()); }


// GratingVelocityWidget ////////////////////////////////////////////////////////////////////

GratingVelocityWidget::GratingVelocityWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), distAdj_(Gtk::Adjustment::create(peer->GetVel(),-100,100)), hzAdj_(Gtk::Adjustment::create(peer->GetVel(),-10,10)), peer_(peer)
{
	gtk::HNumWidget *widget = Gtk::manage(new gtk::HNumWidget("velocity", distAdj_, 0, peer->GetUnits() + units::per_sec));
	widget->AddUnits(units::hz,hzAdj_,2);
	widget->SelectUnits(peer->GetVelUnits());
	append(*widget);
	widget->set_hexpand();
	distAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &GratingVelocityWidget::OnChangedDist));
	hzAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &GratingVelocityWidget::OnChangedHz));
	widget->SignalUnitsChanged().connect(sigc::mem_fun(*this, &GratingVelocityWidget::OnChangedUnits));
}

void GratingVelocityWidget::OnChangedDist() 	{ peer_->SetVel(distAdj_->get_value()); }
void GratingVelocityWidget::OnChangedHz() 	{ peer_->SetVel(hzAdj_->get_value()); }

void GratingVelocityWidget::OnChangedUnits(std::string units)
{
	// TODO: causes a spurious change to peer when slider is updated
	peer_->SetVelUnitsHz(units==units::hz);
	double vel = peer_->GetVel();
	(units==units::hz) ? hzAdj_->set_value(vel) : distAdj_->set_value(vel);
}


// GratingCenterWidget ////////////////////////////////////////////////////////////////////

GratingCenterWidget::GratingCenterWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(100*peer->GetCenter(),100*Grating::center_min,100*Grating::center_max)), peer_(peer)
{
	append(*Gtk::manage(new gtk::HNumWidget("center", adj_, 0, "%")));	adj_->signal_value_changed().connect(
		sigc::mem_fun(*this, &GratingCenterWidget::OnChanged));
}

void GratingCenterWidget::OnChanged()
{
	peer_->SetCenter(adj_->get_value()/100);
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
	
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));

	linearRadio_->set_hexpand();
	arcRadio_->set_hexpand();

	set_child(*box);
	box->append(linearRadio_);
	box->append(arcRadio_);
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
	Gtk::Box(Gtk::Orientation::HORIZONTAL), interpWidget_(peer), centerWidget_(peer), graph_(peer), peer_(peer)
{
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));

	interpWidget_->set_vexpand();
	centerWidget_->set_vexpand();
	box->set_hexpand();
	graph_->set_hexpand();

	box->append(interpWidget_);
	box->append(centerWidget_);
	append(*box);
	append(graph_);

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
	signal_clicked().connect(sigc::mem_fun(*this, &GratingAdvancedButton::OnPush));
};
void GratingAdvancedButton::OnPush() { dialog_.show(); }

// GratingWidget ////////////////////////////////////////////////////////////////////

GratingPitchWidget::GratingPitchWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), ridgeWidget_(NULL), gapWidget_(NULL), regularizeButton_(peer), peer_(peer)
{
	ridgeWidget_ = Gtk::manage(new GratingRidgeSizeWidget(peer));
	gapWidget_ = Gtk::manage(new GratingGapSizeWidget(peer));

	ridgeWidget_->set_hexpand();
	gapWidget_->set_hexpand();

	append(*ridgeWidget_);
	append(*gapWidget_);

	if (peer->GetRegularizable()) append(regularizeButton_);
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

	ridgeWidget_ = Gtk::manage(new GratingRidgeSizeWidget(peer_));
	gapWidget_ = Gtk::manage(new GratingGapSizeWidget(peer_));

	ridgeWidget_->set_hexpand():
	gapWidget_->set_hexpand():

	append(*ridgeWidget_);
	append(*gapWidget_);
	if (peer_->GetRegularizable()) append(regularizeButton_);
	show_all_children();
}

GratingWidget::GratingWidget(GratingPtr peer) :
	gtk::CheckFrame(peer->GetEnable(), "grating"), peer_(peer)
{
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid());
	GetBox().append(*grid);
	grid->attach(*Gtk::manage(new GratingPitchWidget(peer)), 0, 0, 2, 1);
	grid->attach(*Gtk::manage(new GratingVelocityWidget(peer)), 0, 1, 2, 1);
	grid->attach(*Gtk::manage(new GratingAdvancedButton(peer)), 0, 2, 1, 1);
	grid->attach(*Gtk::manage(new GratingInvertCtrl(peer)), 1, 2, 1, 1);
	GetCheck().signal_clicked().connect(sigc::mem_fun(*this, &GratingWidget::OnEnable));
};
void GratingWidget::OnEnable() { peer_->SetEnable(GetCheck().get_active()); }



// AdvancedGratingWidget ////////////////////////////////////////////////////////////////////

AdvancedGratingWidget::AdvancedGratingWidget(GratingPtr peer) :
	Gtk::Dialog("Advanced Grating Options")
{
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	auto rhythmWidget = Gtk::manage(new GratingRhythmWidget(peer));	
	auto cycleWidget = Gtk::manage(new GratingCycleWidget(peer));
	auto modulatorCtrl = Gtk::manage(new GratingModulatorCtrl(peer->GetModulator()));

	box->set_vexpand();
	rhythmWidget->set_hexpand();
	cycleWidget->set_hexpand();
	modulatorCtrl->set_hexpand();

	get_content_area()->append(*box);
	box->append(*rhythmWidget);
	box->append(*cycleWidget);
	box->append(*modulatorCtrl);
	show_all_children();
}

// GratingAmplitudeWidget ////////////////////////////////////////////////////////////////////

GratingAmplitudeWidget::GratingAmplitudeWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetAmplitude()*100,0,100)), peer_(peer)
{
	append(*Gtk::manage(new gtk::HNumWidget("amplitude", adj_, 0, units::percent)));
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &GratingAmplitudeWidget::OnChanged));
}
void GratingAmplitudeWidget::OnChanged() { peer_->SetAmplitude(adj_->get_value()/100); }


} // namespace graphics
} // namespace latero
