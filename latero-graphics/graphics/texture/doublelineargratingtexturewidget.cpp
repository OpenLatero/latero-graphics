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
#include <gtkmm/box.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/frame.h>
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
		add(widget_);
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &SeedCtrl::OnChanged));
	}
	virtual ~SeedCtrl() {};
protected:
	void OnChanged() { peer_->SetSeed(widget_.GetValue()); };
	DoubleLinearGratingTexturePtr peer_;
	PointWidget widget_;
};

class AngleWidget : public Gtk::HBox
{
public:
	AngleWidget(DoubleLinearGratingTexturePtr peer) :
		adj_(Gtk::Adjustment::create(peer->GetAngle(),0,360)), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("orientation", adj_, 0, units::degree)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &AngleWidget::OnChanged));
	}
	virtual ~AngleWidget() {};
protected:
	void OnChanged() { peer_->SetAngle(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class PrimRidgeSizeWidget : public Gtk::HBox
{
public:
	PrimRidgeSizeWidget(DoubleLinearGratingTexturePtr peer) :
		adj_(Gtk::Adjustment::create(peer->GetPrimaryRidgeSize(),0,100)), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("primary ridge size", adj_, 1, units::mm)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PrimRidgeSizeWidget::OnChanged));
	}
	virtual ~PrimRidgeSizeWidget() {};
protected:
	void OnChanged() { peer_->SetPrimaryRidgeSize(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class SecondaryScaleWidget : public Gtk::HBox
{
public:
	SecondaryScaleWidget(DoubleLinearGratingTexturePtr peer) :
		adj_(Gtk::Adjustment::create(peer->GetSecondaryScale(),0,10)), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("secondary scale", adj_, 1)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &SecondaryScaleWidget::OnChanged));
	}
	virtual ~SecondaryScaleWidget() {};
protected:
	void OnChanged() { peer_->SetSecondaryScale(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class PrimGapSizeWidget : public Gtk::HBox
{
public:
	PrimGapSizeWidget(DoubleLinearGratingTexturePtr peer) :
		adj_(Gtk::Adjustment::create(peer->GetPrimaryGapSize(),0,100)), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("primary gap size", adj_, 1, units::mm)));
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
		add(combo_);
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
		add(combo_);
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
		Gtk::HBox *box = manage(new Gtk::HBox);
		add(*box);
		box->pack_start(*manage(new gtk::VNumWidget(adj0_,0, units::percent)), Gtk::PACK_SHRINK);
		box->pack_start(*manage(new gtk::VNumWidget(adj1_,0, units::percent)), Gtk::PACK_SHRINK);
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
	auto grid = manage(new Gtk::Grid());

	auto amplitudeCtrl = manage(new AmplitudeCtrl(peer_));
	auto angleWidget = manage(new AngleWidget(peer_));
	auto operationWidget = manage(new OperationWidget(peer_));
	auto constraintWidget = manage(new ConstraintWidget(peer_));
	auto primRidgeSizeWidget = manage(new PrimRidgeSizeWidget(peer_));
	auto primGapSizeWidget = manage(new PrimGapSizeWidget(peer_));
	auto secondaryScaleWidget = manage(new SecondaryScaleWidget(peer_));
	auto vibCheck = CreateVibCheck();
	auto tDCentricCheck = CreateTDCentricCheck();
	auto motionWidget = CreateMotionWidget();
	
	amplitudeCtrl->set_hexpand(false);
	angleWidget->set_hexpand(true);
	operationWidget->set_hexpand(true);
	constraintWidget->set_hexpand(true);
	primRidgeSizeWidget->set_hexpand(true);
	primGapSizeWidget->set_hexpand(true);
	secondaryScaleWidget->set_hexpand(true);
	vibCheck->set_hexpand(true);
	tDCentricCheck->set_hexpand(true);
	motionWidget->set_hexpand(true);

	grid->attach(*amplitudeCtrl,0,0,1,3);
	grid->attach(*angleWidget,1,0,1,1);
	grid->attach(*operationWidget,2,0,1,1);
	grid->attach(*constraintWidget,3,0,1,1);
	grid->attach(*primRidgeSizeWidget,1,1,1,1);
	grid->attach(*primGapSizeWidget,2,1,1,1);
	grid->attach(*secondaryScaleWidget,3,1,1,1);

	Gtk::HBox *box = manage(new Gtk::HBox);
	box->pack_start(*vibCheck);
	box->pack_start(*tDCentricCheck);
	grid->attach(*box,1,2,1,1);
	grid->attach(*motionWidget,2,2,2,1);
	SetContent(grid,false);
}


DoubleLinearGratingTextureAdvancedWidget::DoubleLinearGratingTextureAdvancedWidget(DoubleLinearGratingTexturePtr peer) :
	peer_(peer)
{
	using namespace DoubleLinearGratingTextureWidgets;

	set_hexpand(true);
	set_vexpand(true);

	auto amplitudeCtrl = manage(new AmplitudeCtrl(peer));
	auto angleWidget = manage(new AngleWidget(peer));
	auto operationWidget = manage(new OperationWidget(peer));
	auto constraintWidget = manage(new ConstraintWidget(peer));
	auto primRidgeSizeWidget = manage(new PrimRidgeSizeWidget(peer));
	auto primGapSizeWidget = manage(new PrimGapSizeWidget(peer));
	auto secondaryScaleWidget = manage(new SecondaryScaleWidget(peer));
	auto seedCtrl = manage(new SeedCtrl(peer));
	auto oscillatorWidget = manage(new OscillatorWidget(peer->GetOscillator(),true));
	auto textureMotionCtrl = manage(new TextureMotionCtrl(peer));
	auto patternPreview = manage(new PatternPreview(peer));

	amplitudeCtrl->set_hexpand(false);
	angleWidget->set_hexpand(true);
	operationWidget->set_hexpand(true);
	constraintWidget->set_hexpand(true);
	primRidgeSizeWidget->set_hexpand(true);
	primGapSizeWidget->set_hexpand(true);
	secondaryScaleWidget->set_hexpand(true);
	seedCtrl->set_hexpand(true);
	oscillatorWidget->set_hexpand(true);
	textureMotionCtrl->set_hexpand(true);
	patternPreview->set_hexpand(false);

	amplitudeCtrl->set_vexpand(true);
	angleWidget->set_vexpand(true);
	operationWidget->set_vexpand(true);
	constraintWidget->set_vexpand(true);
	primRidgeSizeWidget->set_vexpand(true);
	primGapSizeWidget->set_vexpand(true);
	secondaryScaleWidget->set_vexpand(true);
	seedCtrl->set_vexpand(true);
	oscillatorWidget->set_vexpand(true);
	textureMotionCtrl->set_vexpand(true);
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

	show_all_children();
}

} // namespace graphics
} // namespace latero
