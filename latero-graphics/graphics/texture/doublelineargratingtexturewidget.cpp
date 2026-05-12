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

#include "doublelineargratingtexturewidget.h"
#include "doublelineargratingtexture.h"
#include "../../gtk/numwidget.h"
#include "../../pointwidget.h"
#include "../../graphics/patternpreview.h"
#include "texturewidget.h"
#include "../../graphics/groupwidget.h"

namespace latero {
namespace graphics { 

namespace DoubleLinearGratingTextureWidgets {

class SeedCtrl : public Gtk::Frame
{
public:
	SeedCtrl(DoubleLinearGratingTexturePtr peer) :
		Gtk::Frame("seed"),
		peer_(peer),
		widget_(peer->GetSeed(),0,227,0,168) // TODO: we'd like to know the surface size here...
	{
		set_child(widget_);
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &SeedCtrl::OnChanged));
	}
	virtual ~SeedCtrl() {};
protected:
	void OnChanged() { peer_->SetSeed(widget_.GetValue()); };
	DoubleLinearGratingTexturePtr peer_;
	PointWidget widget_;
};

class AngleWidget : public Gtk::Box
{
public:
	AngleWidget(DoubleLinearGratingTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetAngle(),0,360)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::HNumWidget>("orientation", adj_, 0, units::degree));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &AngleWidget::OnChanged));
	}
	virtual ~AngleWidget() {};
protected:
	void OnChanged() { peer_->SetAngle(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class PrimRidgeSizeWidget : public Gtk::Box
{
public:
	PrimRidgeSizeWidget(DoubleLinearGratingTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetPrimaryRidgeSize(),0,100)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::HNumWidget>("primary ridge size", adj_, 1, units::mm));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PrimRidgeSizeWidget::OnChanged));
	}
	virtual ~PrimRidgeSizeWidget() {};
protected:
	void OnChanged() { peer_->SetPrimaryRidgeSize(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class SecondaryScaleWidget : public Gtk::Box
{
public:
	SecondaryScaleWidget(DoubleLinearGratingTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetSecondaryScale(),0,10)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::HNumWidget>("secondary scale", adj_, 1));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &SecondaryScaleWidget::OnChanged));
	}
	virtual ~SecondaryScaleWidget() {};
protected:
	void OnChanged() { peer_->SetSecondaryScale(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class PrimGapSizeWidget : public Gtk::Box
{
public:
	PrimGapSizeWidget(DoubleLinearGratingTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetPrimaryGapSize(),0,100)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::HNumWidget>("primary gap size", adj_, 1, units::mm));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PrimGapSizeWidget::OnChanged));
	}
	virtual ~PrimGapSizeWidget() {};
protected:
	void OnChanged() { peer_->SetPrimaryGapSize(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class ConstraintWidget : public Gtk::Frame
{
public:
	ConstraintWidget(DoubleLinearGratingTexturePtr peer) :
		Gtk::Frame("constraint"),
		peer_(peer)
	{
		set_child(combo_);
		combo_.append("orthogonal");
		combo_.append("parallel");
		if (peer->GetConstraint() == DoubleLinearGratingTexture::constraint_ortho)
			combo_.set_active_text("orthogonal");
		else
			combo_.set_active_text("parallel");
		combo_.signal_changed().connect(sigc::mem_fun(*this, &ConstraintWidget::OnChange));
	}
	virtual ~ConstraintWidget() {};
protected:
	void OnChange()
	{
		if (combo_.get_active_text() == "orthogonal")
			peer_->SetConstraint(DoubleLinearGratingTexture::constraint_ortho);
		else
			peer_->SetConstraint(DoubleLinearGratingTexture::constraint_parallel);
	};
	Gtk::ComboBoxText combo_;
	DoubleLinearGratingTexturePtr peer_;
};

class OperationWidget : public Gtk::Frame
{
public:
	OperationWidget(DoubleLinearGratingTexturePtr peer) :
		Gtk::Frame("operation"),
		combo_(peer)
	{
		set_child(combo_);
	}
	virtual ~OperationWidget() {};
protected:
	GroupOpCombo combo_;
};

class AmplitudeCtrl : public Gtk::Frame
{
public:
	AmplitudeCtrl(DoubleLinearGratingTexturePtr peer) :
		Gtk::Frame("amplitude"),
		adj0_(Gtk::Adjustment::create(peer->GetPrimaryAmplitude()*100,0,100)),
		adj1_(Gtk::Adjustment::create(peer->GetSecondaryAmplitude()*100,0,100)),
        peer_(peer)
	{
		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
		set_child(*box);
		box->append(*Gtk::make_managed<gtk::VNumWidget>(adj0_,0, units::percent));
		box->append(*Gtk::make_managed<gtk::VNumWidget>(adj1_,0, units::percent));
		adj0_->signal_value_changed().connect(sigc::mem_fun(*this, &AmplitudeCtrl::OnChanged0));
		adj1_->signal_value_changed().connect(sigc::mem_fun(*this, &AmplitudeCtrl::OnChanged1));
	}
	virtual ~AmplitudeCtrl() {};
protected:
	void OnChanged0() { peer_->SetPrimaryAmplitude(adj0_->get_value()/100); }
	void OnChanged1() { peer_->SetSecondaryAmplitude(adj1_->get_value()/100); }
    Glib::RefPtr<Gtk::Adjustment> adj0_, adj1_;
	DoubleLinearGratingTexturePtr peer_;
};


} // namespace

void DoubleLinearGratingTextureWidget::Create()
{
	using namespace DoubleLinearGratingTextureWidgets;
	auto grid = Gtk::make_managed<Gtk::Grid>();

	auto amplitudeCtrl = Gtk::make_managed<AmplitudeCtrl>(peer_);
	auto angleWidget = Gtk::make_managed<AngleWidget>(peer_);
	auto operationWidget = Gtk::make_managed<OperationWidget>(peer_);
	auto constraintWidget = Gtk::make_managed<ConstraintWidget>(peer_);
	auto primRidgeSizeWidget = Gtk::make_managed<PrimRidgeSizeWidget>(peer_);
	auto primGapSizeWidget = Gtk::make_managed<PrimGapSizeWidget>(peer_);
	auto secondaryScaleWidget = Gtk::make_managed<SecondaryScaleWidget>(peer_);
	auto vibCheck = CreateVibCheck();
	auto tDCentricCheck = CreateTDCentricCheck();
	auto motionWidget = CreateMotionWidget();
	
	amplitudeCtrl->set_hexpand(false);
	angleWidget->set_hexpand();
	operationWidget->set_hexpand();
	constraintWidget->set_hexpand();
	primRidgeSizeWidget->set_hexpand();
	primGapSizeWidget->set_hexpand();
	secondaryScaleWidget->set_hexpand();
	vibCheck->set_hexpand();
	tDCentricCheck->set_hexpand();
	motionWidget->set_hexpand();

	grid->attach(*amplitudeCtrl,0,0,1,3);
	grid->attach(*angleWidget,1,0,1,1);
	grid->attach(*operationWidget,2,0,1,1);
	grid->attach(*constraintWidget,3,0,1,1);
	grid->attach(*primRidgeSizeWidget,1,1,1,1);
	grid->attach(*primGapSizeWidget,2,1,1,1);
	grid->attach(*secondaryScaleWidget,3,1,1,1);

	vibCheck->set_hexpand();
	tDCentricCheck->set_hexpand();

	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	box->append(*vibCheck);
	box->append(*tDCentricCheck);
	grid->attach(*box,1,2,1,1);
	grid->attach(*motionWidget,2,2,2,1);
	SetContent(grid,false);
}


DoubleLinearGratingTextureAdvancedWidget::DoubleLinearGratingTextureAdvancedWidget(DoubleLinearGratingTexturePtr peer) :
	peer_(peer)
{
	using namespace DoubleLinearGratingTextureWidgets;

	set_hexpand();
	set_vexpand();

	auto amplitudeCtrl = Gtk::make_managed<AmplitudeCtrl>(peer);
	auto angleWidget = Gtk::make_managed<AngleWidget>(peer);
	auto operationWidget = Gtk::make_managed<OperationWidget>(peer);
	auto constraintWidget = Gtk::make_managed<ConstraintWidget>(peer);
	auto primRidgeSizeWidget = Gtk::make_managed<PrimRidgeSizeWidget>(peer);
	auto primGapSizeWidget = Gtk::make_managed<PrimGapSizeWidget>(peer);
	auto secondaryScaleWidget = Gtk::make_managed<SecondaryScaleWidget>(peer);
	auto seedCtrl = Gtk::make_managed<SeedCtrl>(peer);
	auto oscillatorWidget = Gtk::make_managed<OscillatorWidget>(peer->GetOscillator(),true);
	auto textureMotionCtrl = Gtk::make_managed<TextureMotionCtrl>(peer);
	auto patternPreview = Gtk::make_managed<PatternPreview>(peer);

	amplitudeCtrl->set_hexpand(false);
	angleWidget->set_hexpand();
	operationWidget->set_hexpand();
	constraintWidget->set_hexpand();
	primRidgeSizeWidget->set_hexpand();
	primGapSizeWidget->set_hexpand();
	secondaryScaleWidget->set_hexpand();
	seedCtrl->set_hexpand();
	oscillatorWidget->set_hexpand();
	textureMotionCtrl->set_hexpand();
	patternPreview->set_hexpand(false);

	amplitudeCtrl->set_vexpand();
	angleWidget->set_vexpand();
	operationWidget->set_vexpand();
	constraintWidget->set_vexpand();
	primRidgeSizeWidget->set_vexpand();
	primGapSizeWidget->set_vexpand();
	secondaryScaleWidget->set_vexpand();
	seedCtrl->set_vexpand();
	oscillatorWidget->set_vexpand();
	textureMotionCtrl->set_vexpand();
	patternPreview->set_vexpand(false);

	attach(*amplitudeCtrl,0,0,1,5);
	attach(*angleWidget,1,0,1,1);
	attach(*operationWidget,2,0,1,1);
	attach(*constraintWidget,3,0,1,1);
	attach(*primRidgeSizeWidget,1,1,1,1);
	attach(*primGapSizeWidget,2,1,1,1);
	attach(*secondaryScaleWidget,3,1,1,1);
	attach(*seedCtrl,1,2,3,1);
	attach(*oscillatorWidget,1,3,3,1);
	attach(*textureMotionCtrl,1,4,3,1);
	attach(*patternPreview,4,0,1,5);
}

} // namespace graphics
} // namespace latero
