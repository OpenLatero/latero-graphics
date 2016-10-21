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

#include "polygonwidget.h"
#include <iostream>
#include "polygon.h"
#include "strokewidget.h"
#include "../pointwidget.h"
#include <gtkmm/frame.h>
#include "../gtk/numwidget.h"
#include "../gtk/checkframe.h"
#include "../oscillatorwidget.h"
#include "../pointlistwidget.h"
#include "texture/textureselectorwidget.h"
#include "patternpreview.h"
#include "closedpatternwidget.h"

namespace latero {
namespace graphics { 

class PolygonJoinTypeCombo : public Gtk::ComboBoxText
{
public:
	PolygonJoinTypeCombo(PolygonPtr peer) : peer_(peer)
	{
		Polygon::JoinTypeSet ops = peer->GetJoinTypes();
		for (unsigned int i=0; i<ops.size(); ++i)
			append_text(ops[i].label);
		set_active_text(peer->GetJoinType().label);
		signal_changed().connect(sigc::mem_fun(*this, &PolygonJoinTypeCombo::OnChange));
	};
	virtual ~PolygonJoinTypeCombo() {};
private:
	void OnChange() { peer_->SetJoinType(get_active_text()); };
	PolygonPtr peer_;
};


class PolygonRoundingOffsetCtrl : public Gtk::VBox
{
public:
	PolygonRoundingOffsetCtrl(PolygonPtr peer) : 
        adj_(peer->GetRoundingOffset(), 0, 50), peer_(peer)
	{
		add(*manage(new latero::graphics::gtk::HNumWidget("rounding offset", adj_, 1, "mm")));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &PolygonRoundingOffsetCtrl::OnChanged));
	}
	virtual ~PolygonRoundingOffsetCtrl() {};
protected:
	void OnChanged() { peer_->SetRoundingOffset(adj_.get_value()); }
	Gtk::Adjustment adj_;	
	PolygonPtr peer_;
};

class PolygonCornerBlendSizeCtrl : public Gtk::VBox
{
public:
	PolygonCornerBlendSizeCtrl(PolygonPtr peer) : 
		adj_(peer->GetCornerBlendSize(), 0, 10), peer_(peer)
	{
		add(*manage(new latero::graphics::gtk::HNumWidget("blending size", adj_, 1, "mm")));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &PolygonCornerBlendSizeCtrl::OnChanged));
	}
	virtual ~PolygonCornerBlendSizeCtrl() {};
protected:
	void OnChanged() { peer_->SetCornerBlendSize(adj_.get_value()); }
	Gtk::Adjustment adj_;	
	PolygonPtr peer_;
};



class PolygonJoinTypeCtrl : public Gtk::Frame
{
public:
	PolygonJoinTypeCtrl(PolygonPtr peer) : 
		Gtk::Frame("join"), combo_(peer), offsetCtrl_(peer), comboFrame_("type"), peer_(peer)
	{
		add(box_);
		box_.pack_start(comboFrame_, Gtk::PACK_SHRINK);
		comboFrame_.add(combo_);
		box_.pack_start(offsetCtrl_);
		OnChange();
		combo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonJoinTypeCtrl::OnChange));
	}
	virtual ~PolygonJoinTypeCtrl() {}
protected:
	void OnChange() { offsetCtrl_.set_sensitive(peer_->GetJoinType()==Polygon::join_type_round); }

	Gtk::HBox box_;
	PolygonJoinTypeCombo combo_;
	PolygonRoundingOffsetCtrl offsetCtrl_;
	Gtk::Frame comboFrame_;
	PolygonPtr peer_;
};





class PolygonCornerSpanCombo : public Gtk::ComboBoxText
{
public:
	PolygonCornerSpanCombo(PolygonPtr peer) : peer_(peer)
	{
		Polygon::CornerSpanSet ops = peer->GetCornerSpans();
		for (unsigned int i=0; i<ops.size(); ++i)
			append_text(ops[i].label);
		set_active_text(peer->GetCornerSpan().label);
		signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerSpanCombo::OnChange));
	};
	virtual ~PolygonCornerSpanCombo() {};
private:
	void OnChange() { peer_->SetCornerSpan(get_active_text()); };
	PolygonPtr peer_;
};

class PolygonCornerBlendCombo : public Gtk::Frame
{
public:
	PolygonCornerBlendCombo(PolygonPtr peer) : Gtk::Frame("blend"), peer_(peer)
	{
		add(combo_);
		Polygon::CornerBlendSet ops = peer->GetCornerBlends();
		for (unsigned int i=0; i<ops.size(); ++i)
			combo_.append_text(ops[i].label);
		combo_.set_active_text(peer->GetCornerBlend().label);
		combo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerBlendCombo::OnChange));
	};
	virtual ~PolygonCornerBlendCombo() {};
private:
	Gtk::ComboBoxText combo_;
	void OnChange() { peer_->SetCornerBlend(combo_.get_active_text()); };
	PolygonPtr peer_;
};

class PolygonCornerTypeCombo : public Gtk::Frame
{
public:
	PolygonCornerTypeCombo(PolygonPtr peer) : Gtk::Frame("type"), peer_(peer)
	{
		add(combo_);
		Polygon::CornerTypeSet ops = peer->GetCornerTypes();
		for (unsigned int i=0; i<ops.size(); ++i)
			combo_.append_text(ops[i].label);
		combo_.set_active_text(peer->GetCornerType().label);
		combo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerTypeCombo::OnChange));
	};
	virtual ~PolygonCornerTypeCombo() {};
	Glib::SignalProxy0<void> signal_changed() { return combo_.signal_changed(); }
private:
	Gtk::ComboBoxText combo_;
	void OnChange() { peer_->SetCornerType(combo_.get_active_text()); };
	PolygonPtr peer_;
};


class PolygonCornerAngleCtrl : public Gtk::Frame
{
public:
	PolygonCornerAngleCtrl(PolygonPtr peer) : 
		Gtk::Frame("angle"), 
		adj_(peer->GetCornerUserAngle(), 0, 180),
		angleCtrl_(adj_), 
        combo_(peer), 
        peer_(peer)
	{
		angleCtrl_.set_sensitive(peer->GetCornerSpan()==Polygon::corner_span_user );
		Gtk::HBox *box = manage(new Gtk::HBox);
		box->pack_start(combo_, Gtk::PACK_SHRINK);
		box->pack_start(angleCtrl_);
		add(*box);
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &PolygonCornerAngleCtrl::OnChanged));
		combo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerAngleCtrl::OnComboChanged));
	}
	virtual ~PolygonCornerAngleCtrl() {};
protected:
	void OnChanged() { peer_->SetCornerUserAngle(adj_.get_value()); }
	void OnComboChanged() { angleCtrl_.set_sensitive(peer_->GetCornerSpan()==Polygon::corner_span_user); }

	Gtk::Adjustment adj_;	
	Gtk::SpinButton angleCtrl_;
	PolygonCornerSpanCombo combo_;
	PolygonPtr peer_;
};



class PolygonCornerMotionCheck : public Gtk::CheckButton
{
public:
	PolygonCornerMotionCheck(PolygonPtr peer) : Gtk::CheckButton("motion"), peer_(peer)
	{
		set_active(peer->GetCornerMotion());
		signal_clicked().connect(sigc::mem_fun(*this, &PolygonCornerMotionCheck::OnChange));
	};
	virtual ~PolygonCornerMotionCheck() {};
private:
	void OnChange() { peer_->SetCornerMotion(get_active()); };
	PolygonPtr peer_;
};

class PolygonCornerCtrl : public gtk::CheckFrame
{
public:
	PolygonCornerCtrl(PolygonPtr peer) : 
		gtk::CheckFrame(peer->GetCornerEnable(), "corner marker", true), 
		angleCtrl_(peer), typeCombo_(peer), vibWidget_(peer->GetCornerOscillator()), peer_(peer)
	{
		
		vibWidget_.set_sensitive(peer_->GetCornerType()==Polygon::corner_type_solid);

		GetBox().pack_start(mainbox_);
		mainbox_.pack_start(upperbox_);
		mainbox_.pack_start(lowerbox_);
		upperbox_.pack_start(angleCtrl_);
		upperbox_.pack_start(*manage(new PolygonCornerBlendCombo(peer)));
		upperbox_.pack_start(*manage(new PolygonCornerBlendSizeCtrl(peer)));
		upperbox_.pack_start(*manage(new PolygonCornerMotionCheck(peer)));
		lowerbox_.pack_start(typeCombo_);
		lowerbox_.pack_start(vibWidget_);
		GetCheck().signal_clicked().connect(sigc::mem_fun(*this, &PolygonCornerCtrl::OnClick));
		typeCombo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerCtrl::OnComboChanged));
	}
	virtual ~PolygonCornerCtrl() {};
protected:
	void OnClick() { peer_->SetCornerEnable(GetCheck().get_active()); };
	void OnComboChanged() { vibWidget_.set_sensitive(peer_->GetCornerType()==Polygon::corner_type_solid); }

    PolygonCornerAngleCtrl angleCtrl_;
	PolygonCornerTypeCombo typeCombo_;
	OscillatorWidget vibWidget_;
	Gtk::VBox mainbox_;
	Gtk::HBox upperbox_;
	Gtk::HBox lowerbox_;
    PolygonPtr peer_;
};

class PolygonVerticesCtrl : public Gtk::Frame
{
public:
	PolygonVerticesCtrl(PolygonPtr peer) : Gtk::Frame("vertices"), list_(peer->GetPoints()), peer_(peer)
	{
		add(list_);
		list_.SignalChanged().connect(sigc::mem_fun(*this, &PolygonVerticesCtrl::OnChanged));
	}
	virtual ~PolygonVerticesCtrl() {};

protected:
	void OnChanged() { peer_->SetPoints(list_.GetPoints()); }

	PointListWidget list_;
	PolygonPtr peer_;
};

class PolygonPropertiesCtrl : public Gtk::HBox
{
public:
	PolygonPropertiesCtrl(PolygonPtr peer)
	{
		pack_start(box_);
		box_.pack_start(*manage(new PolygonJoinTypeCtrl(peer)), Gtk::PACK_SHRINK);
		box_.pack_start(*manage(new PolygonCornerCtrl(peer)), Gtk::PACK_SHRINK);
		pack_start(*manage(new PolygonVerticesCtrl(peer)), Gtk::PACK_SHRINK);
		pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);
	}
	virtual ~PolygonPropertiesCtrl() {};
protected:
	Gtk::VBox box_;
};


PolygonWidget::PolygonWidget(PolygonPtr peer)
{
	append_page(*manage(new PolygonPropertiesCtrl(peer)), "properties");
	append_page(*manage(new StrokeProfileWidget(peer->GetStroke())), "stroke");
	append_page(*manage(new StrokeFillWidget(peer->GetStroke())), "fill");
	append_page(*manage(new StrokeMotionWidget(peer->GetStroke())),"motion");
	append_page(*manage(new StrokeDottedWidget(peer->GetStroke())),"dots");
	append_page(*manage(new ClosedPatternFillTextureWidget(peer)),"texture");
}

} // namespace graphics
} // namespace latero

