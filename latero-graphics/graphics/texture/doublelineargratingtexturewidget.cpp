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
		adj_(peer->GetAngle(),0,360), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("orientation", adj_, 0, units::degree)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &AngleWidget::OnChanged));
	}
	virtual ~AngleWidget() {};
protected:
	void OnChanged() { peer_->SetAngle(adj_.get_value()); }
	Gtk::Adjustment adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class PrimRidgeSizeWidget : public Gtk::HBox
{
public:
	PrimRidgeSizeWidget(DoubleLinearGratingTexturePtr peer) :
		adj_(peer->GetPrimaryRidgeSize(),0,100), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("primary ridge size", adj_, 1, units::mm)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &PrimRidgeSizeWidget::OnChanged));
	}
	virtual ~PrimRidgeSizeWidget() {};
protected:
	void OnChanged() { peer_->SetPrimaryRidgeSize(adj_.get_value()); }
	Gtk::Adjustment adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class SecondaryScaleWidget : public Gtk::HBox
{
public:
	SecondaryScaleWidget(DoubleLinearGratingTexturePtr peer) :
		adj_(peer->GetSecondaryScale(),0,10), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("secondary scale", adj_, 1)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &SecondaryScaleWidget::OnChanged));
	}
	virtual ~SecondaryScaleWidget() {};
protected:
	void OnChanged() { peer_->SetSecondaryScale(adj_.get_value()); }
	Gtk::Adjustment adj_;
	DoubleLinearGratingTexturePtr peer_;
};

class PrimGapSizeWidget : public Gtk::HBox
{
public:
	PrimGapSizeWidget(DoubleLinearGratingTexturePtr peer) :
		adj_(peer->GetPrimaryGapSize(),0,100), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("primary gap size", adj_, 1, units::mm)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &PrimGapSizeWidget::OnChanged));
	}
	virtual ~PrimGapSizeWidget() {};
protected:
	void OnChanged() { peer_->SetPrimaryGapSize(adj_.get_value()); }
	Gtk::Adjustment adj_;
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
		combo_.append_text("orthogonal");
		combo_.append_text("parallel");
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
		adj0_(peer->GetPrimaryAmplitude()*100,0,100),
		adj1_(peer->GetSecondaryAmplitude()*100,0,100),
        peer_(peer)
	{
		Gtk::HBox *box = manage(new Gtk::HBox);
		add(*box);
		box->pack_start(*manage(new gtk::VNumWidget(adj0_,0, units::percent)), Gtk::PACK_SHRINK);
		box->pack_start(*manage(new gtk::VNumWidget(adj1_,0, units::percent)), Gtk::PACK_SHRINK);
		adj0_.signal_value_changed().connect(sigc::mem_fun(*this, &AmplitudeCtrl::OnChanged0));
		adj1_.signal_value_changed().connect(sigc::mem_fun(*this, &AmplitudeCtrl::OnChanged1));
	}
	virtual ~AmplitudeCtrl() {};
protected:
	void OnChanged0() { peer_->SetPrimaryAmplitude(adj0_.get_value()/100); }
	void OnChanged1() { peer_->SetSecondaryAmplitude(adj1_.get_value()/100); }
	Gtk::Adjustment adj0_, adj1_;
	DoubleLinearGratingTexturePtr peer_;
};


} // namespace

void DoubleLinearGratingTextureWidget::Create()
{
	using namespace DoubleLinearGratingTextureWidgets;
	Gtk::Table *table = manage(new Gtk::Table(4,3));
	table->attach(*manage(new AmplitudeCtrl(peer_)),	0,1,0,3, Gtk::SHRINK);

	table->attach(*manage(new AngleWidget(peer_)),		1,2,0,1);
	table->attach(*manage(new OperationWidget(peer_)),	2,3,0,1);
	table->attach(*manage(new ConstraintWidget(peer_)),	3,4,0,1);

	table->attach(*manage(new PrimRidgeSizeWidget(peer_)),	1,2,1,2);
	table->attach(*manage(new PrimGapSizeWidget(peer_)),	2,3,1,2);
	table->attach(*manage(new SecondaryScaleWidget(peer_)),	3,4,1,2);

	Gtk::HBox *box = manage(new Gtk::HBox);
	box->pack_start(*CreateVibCheck());
	box->pack_start(*CreateTDCentricCheck());
	table->attach(*box,		 			1,2,2,3);
	table->attach(*CreateMotionWidget(), 			2,4,2,3);
	SetContent(table,false);
}


DoubleLinearGratingTextureAdvancedWidget::DoubleLinearGratingTextureAdvancedWidget(DoubleLinearGratingTexturePtr peer) :
	Gtk::Table(5,5),
	peer_(peer)
{
	using namespace DoubleLinearGratingTextureWidgets;

	attach(*manage(new AmplitudeCtrl(peer)),	0,1,0,5, Gtk::SHRINK);
	attach(*manage(new AngleWidget(peer)),		1,2,0,1);
	attach(*manage(new OperationWidget(peer)),	2,3,0,1);
	attach(*manage(new ConstraintWidget(peer)),	3,4,0,1);
	attach(*manage(new PrimRidgeSizeWidget(peer)),	1,2,1,2);
	attach(*manage(new PrimGapSizeWidget(peer)),	2,3,1,2);
	attach(*manage(new SecondaryScaleWidget(peer)),	3,4,1,2);
	attach(*manage(new SeedCtrl(peer)),		1,4,2,3);
	attach(*manage(new OscillatorWidget(peer->GetOscillator(),true)), 	1,4,3,4);
	attach(*manage(new TextureMotionCtrl(peer)), 	1,4,4,5);

	attach(*manage(new PatternPreview(peer)), 	4,5,0,5);

	show_all_children();
}

} // namespace graphics
} // namespace latero
