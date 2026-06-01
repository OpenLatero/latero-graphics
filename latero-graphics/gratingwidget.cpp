#include "gratingwidget.h"
#include "grating.h"
#include "gtk/numwidget.h"
#include <iostream>

namespace latero::graphics {

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

	auto plusButton = Gtk::make_managed<Gtk::Button>("+");
	plusButton->signal_clicked().connect(
		sigc::mem_fun(*this, &GratingRhythmWidget::OnAddCycle));
	buttonBox_.append(*plusButton);

	auto minusButton = Gtk::make_managed<Gtk::Button>("-");
	minusButton->signal_clicked().connect(
		sigc::mem_fun(*this, &GratingRhythmWidget::OnRemoveCycle));
	buttonBox_.append(*minusButton);

	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
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

	adjVec_.pop_back();
}

void GratingRhythmWidget::AppendCycle(double value)
{
    auto adj = Gtk::Adjustment::create(value, 0, 100, 10);
	adjVec_.push_back(adj);
	adj->signal_value_changed().connect(sigc::mem_fun(*this, &GratingRhythmWidget::OnChanged));

	auto widget = Gtk::make_managed<latero::graphics::gtk::HVNumWidget>(Gtk::Orientation::VERTICAL, adj,0);
	widget_.push_back(widget);
	sliderBox_.append(*widget);
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
	append(*Gtk::make_managed<gtk::HVNumWidget>(Gtk::Orientation::HORIZONTAL, "gap size", adj_, 2, peer->GetUnits()));
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &GratingGapSizeWidget::OnChanged));
}
void GratingGapSizeWidget::OnChanged() { peer_->SetGapSize(adj_->get_value()); }


// GratingRidgeSizeWidget ////////////////////////////////////////////////////////////////////

GratingRidgeSizeWidget::GratingRidgeSizeWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetRidgeSize(),0,100)), peer_(peer)
{
	append(*Gtk::make_managed<gtk::HVNumWidget>(Gtk::Orientation::HORIZONTAL, "ridge size", adj_, 2, peer->GetUnits()));
	adj_->signal_value_changed().connect(
		sigc::mem_fun(*this, &GratingRidgeSizeWidget::OnChanged));
}
void GratingRidgeSizeWidget::OnChanged() { peer_->SetRidgeSize(adj_->get_value()); }


// GratingVelocityWidget ////////////////////////////////////////////////////////////////////

GratingVelocityWidget::GratingVelocityWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), distAdj_(Gtk::Adjustment::create(peer->GetVel(),-100,100)), hzAdj_(Gtk::Adjustment::create(peer->GetVel(),-10,10)), peer_(peer)
{
	gtk::HVNumWidget *widget = Gtk::make_managed<gtk::HVNumWidget>(Gtk::Orientation::HORIZONTAL, "velocity", distAdj_, 0, peer->GetUnits() + units::per_sec);
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
	append(*Gtk::make_managed<gtk::HVNumWidget>(Gtk::Orientation::HORIZONTAL,"center", adj_, 0, "%"));	adj_->signal_value_changed().connect(
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

	linearRadio_.set_group(arcRadio_);

	Grating::InterpMode mode = peer_->GetInterp();
	if (mode == Grating::interp_linear)
		linearRadio_.set_active();
	else // if (mode == Grating::interp_arc)
		arcRadio_.set_active();
	
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);

	linearRadio_.set_hexpand();
	arcRadio_.set_hexpand();

	set_child(*box);
	box->append(linearRadio_);
	box->append(arcRadio_);
	linearRadio_.signal_toggled().connect(
		sigc::mem_fun(*this, &GratingInterpWidget::OnChanged));
	//arcRadio_.signal_toggled().connect(
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
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);

	interpWidget_.set_vexpand();
	centerWidget_.set_vexpand();
	box->set_hexpand();
	graph_.set_hexpand();

	box->append(interpWidget_);
	box->append(centerWidget_);
	append(*box);
	append(graph_);

	graph_.Refresh();
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
void GratingAdvancedButton::OnPush()
{
	if (auto win = dynamic_cast<Gtk::Window*>(get_root()))
		dialog_.set_transient_for(*win);
	dialog_.set_visible();
}

// GratingWidget ////////////////////////////////////////////////////////////////////

GratingPitchWidget::GratingPitchWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), ridgeWidget_(NULL), gapWidget_(NULL), regularizeButton_(peer), peer_(peer)
{
	ridgeWidget_ = Gtk::make_managed<GratingRidgeSizeWidget>(peer);
	gapWidget_ = Gtk::make_managed<GratingGapSizeWidget>(peer);

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

	ridgeWidget_ = Gtk::make_managed<GratingRidgeSizeWidget>(peer_);
	gapWidget_ = Gtk::make_managed<GratingGapSizeWidget>(peer_);

	ridgeWidget_->set_hexpand();
	gapWidget_->set_hexpand();

	append(*ridgeWidget_);
	append(*gapWidget_);
	if (peer_->GetRegularizable()) append(regularizeButton_);
}

GratingWidget::GratingWidget(GratingPtr peer) :
	gtk::CheckFrame(peer->GetEnable(), "grating"), peer_(peer)
{
	Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
	grid->set_hexpand(true);
	GetBox().append(*grid);
	grid->attach(*Gtk::make_managed<GratingPitchWidget>(peer), 0, 0, 2, 1);
	grid->attach(*Gtk::make_managed<GratingVelocityWidget>(peer), 0, 1, 2, 1);
	grid->attach(*Gtk::make_managed<GratingAdvancedButton>(peer), 0, 2, 1, 1);
	grid->attach(*Gtk::make_managed<GratingInvertCtrl>(peer), 1, 2, 1, 1);
	GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &GratingWidget::OnEnable));
};
void GratingWidget::OnEnable() { peer_->SetEnable(GetCheck().get_active()); }



// AdvancedGratingWidget ////////////////////////////////////////////////////////////////////

AdvancedGratingWidget::AdvancedGratingWidget(GratingPtr peer) :
	Gtk::Window()
{
	set_title("Advanced Grating Options");
	set_modal(true);
	set_hide_on_close(true);
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	auto rhythmWidget = Gtk::make_managed<GratingRhythmWidget>(peer);
	auto cycleWidget = Gtk::make_managed<GratingCycleWidget>(peer);
	auto modulatorCtrl = Gtk::make_managed<GratingModulatorCtrl>(peer->GetModulator());

	box->set_vexpand();
	rhythmWidget->set_hexpand();
	cycleWidget->set_hexpand();
	modulatorCtrl->set_hexpand();

	set_child(*box);
	box->append(*rhythmWidget);
	box->append(*cycleWidget);
	box->append(*modulatorCtrl);
}

// GratingAmplitudeWidget ////////////////////////////////////////////////////////////////////

GratingAmplitudeWidget::GratingAmplitudeWidget(GratingPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetAmplitude()*100,0,100)), peer_(peer)
{
	append(*Gtk::make_managed<gtk::HVNumWidget>(Gtk::Orientation::HORIZONTAL,"amplitude", adj_, 0, units::percent));
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &GratingAmplitudeWidget::OnChanged));
}
void GratingAmplitudeWidget::OnChanged() { peer_->SetAmplitude(adj_->get_value()/100); }


} // namespace
