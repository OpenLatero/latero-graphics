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
			append(ops[i].label);
		set_active_text(peer->GetJoinType().label);
		signal_changed().connect(sigc::mem_fun(*this, &PolygonJoinTypeCombo::OnChange));
	};
	virtual ~PolygonJoinTypeCombo() {};
private:
	void OnChange() { peer_->SetJoinType(get_active_text()); };
	PolygonPtr peer_;
};


class PolygonRoundingOffsetCtrl : public Gtk::Box
{
public:
	PolygonRoundingOffsetCtrl(PolygonPtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetRoundingOffset(), 0, 50)), peer_(peer)
	{
		append(*Gtk::make_managed<latero::graphics::gtk::HNumWidget>("rounding offset", adj_, 1, "mm"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PolygonRoundingOffsetCtrl::OnChanged));
	}
	virtual ~PolygonRoundingOffsetCtrl() {};
protected:
	void OnChanged() { peer_->SetRoundingOffset(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	PolygonPtr peer_;
};

class PolygonCornerBlendSizeCtrl : public Gtk::Box
{
public:
	PolygonCornerBlendSizeCtrl(PolygonPtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetCornerBlendSize(), 0, 10)), peer_(peer)
	{
		auto widget = Gtk::make_managed<latero::graphics::gtk::HNumWidget>("blending size", adj_, 1, "mm");
		widget->set_vexpand(true);
		append(*widget);
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PolygonCornerBlendSizeCtrl::OnChanged));
	}
	virtual ~PolygonCornerBlendSizeCtrl() {};
protected:
	void OnChanged() { peer_->SetCornerBlendSize(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	PolygonPtr peer_;
};



class PolygonJoinTypeCtrl : public Gtk::Frame
{
public:
	PolygonJoinTypeCtrl(PolygonPtr peer) : 
		box_(Gtk::Orientation::HORIZONTAL), Gtk::Frame("join"), combo_(peer), offsetCtrl_(peer), comboFrame_("type"), peer_(peer)
	{
		set_child(box_);
		box_.append(comboFrame_);
		comboFrame_.set_child(combo_);
		offsetCtrl_.set_hexpand();
		box_.append(offsetCtrl_);
		OnChange();
		combo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonJoinTypeCtrl::OnChange));
	}
	virtual ~PolygonJoinTypeCtrl() {}
protected:
	void OnChange() { offsetCtrl_.set_sensitive(peer_->GetJoinType()==Polygon::join_type_round); }

	Gtk::Box box_;
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
			append(ops[i].label);
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
		opsList_ = Gtk::StringList::create({});
		Polygon::CornerBlendSet ops = peer->GetCornerBlends();
		for (unsigned int i=0; i<ops.size(); ++i)
			opsList_->append(ops[i].label);
		opsDropDown_ = Gtk::make_managed<Gtk::DropDown>(opsList_);
		Glib::ustring target = peer->GetCornerBlend().label;
		for (guint i = 0; i < opsList_->get_n_items(); ++i)
			if (opsList_->get_string(i) == target) { opsDropDown_->set_selected(i); break; }
		opsDropDown_->property_selected().signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerBlendCombo::OnChange));
		set_child(*opsDropDown_);
	};
	virtual ~PolygonCornerBlendCombo() {};
private:
	Glib::RefPtr<Gtk::StringList> opsList_;
	Gtk::DropDown* opsDropDown_;
	void OnChange() { peer_->SetCornerBlend(std::string(opsList_->get_string(opsDropDown_->get_selected()))); };
	PolygonPtr peer_;
};

class PolygonCornerTypeCombo : public Gtk::Frame
{
public:
	PolygonCornerTypeCombo(PolygonPtr peer) : Gtk::Frame("type"), peer_(peer)
	{
		set_child(combo_);
		Polygon::CornerTypeSet ops = peer->GetCornerTypes();
		for (unsigned int i=0; i<ops.size(); ++i)
			combo_.append(ops[i].label);
		combo_.set_active_text(peer->GetCornerType().label);
		combo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerTypeCombo::OnChange));
	};
	virtual ~PolygonCornerTypeCombo() {};
	Glib::SignalProxy<void()> signal_changed() { return combo_.signal_changed(); };
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
		adj_(Gtk::Adjustment::create(peer->GetCornerUserAngle(), 0, 180)),
		angleCtrl_(adj_),
        combo_(peer), 
        peer_(peer)
	{
		angleCtrl_.set_sensitive(peer->GetCornerSpan()==Polygon::corner_span_user );
		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
		box->append(combo_);
		angleCtrl_.set_hexpand();
		box->append(angleCtrl_);
		set_child(*box);
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PolygonCornerAngleCtrl::OnChanged));
		combo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerAngleCtrl::OnComboChanged));
	}
	virtual ~PolygonCornerAngleCtrl() {};
protected:
	void OnChanged() { peer_->SetCornerUserAngle(adj_->get_value()); }
	void OnComboChanged() { angleCtrl_.set_sensitive(peer_->GetCornerSpan()==Polygon::corner_span_user); }

    Glib::RefPtr<Gtk::Adjustment> adj_;	
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
		signal_toggled().connect(sigc::mem_fun(*this, &PolygonCornerMotionCheck::OnChange));
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
		upperbox_(Gtk::Orientation::HORIZONTAL), lowerbox_(Gtk::Orientation::HORIZONTAL), mainbox_(Gtk::Orientation::VERTICAL), gtk::CheckFrame(peer->GetCornerEnable(), "corner marker", true), 
		angleCtrl_(peer), typeCombo_(peer), vibWidget_(peer->GetCornerOscillator()), peer_(peer)
	{
		vibWidget_.set_sensitive(peer_->GetCornerType()==Polygon::corner_type_solid);

		auto blendCombo = Gtk::make_managed<PolygonCornerBlendCombo>(peer);
		auto blendSizeCtrl = Gtk::make_managed<PolygonCornerBlendSizeCtrl>(peer);
		auto motionCheck = Gtk::make_managed<PolygonCornerMotionCheck>(peer);

		mainbox_.set_hexpand();
		upperbox_.set_vexpand();
		lowerbox_.set_vexpand();
		angleCtrl_.set_hexpand();
		blendCombo->set_hexpand();
		blendSizeCtrl->set_hexpand();
		motionCheck->set_hexpand();
		typeCombo_.set_hexpand();
		vibWidget_.set_hexpand();

		GetBox().append(mainbox_);
		mainbox_.append(upperbox_);
		mainbox_.append(lowerbox_);
		upperbox_.append(angleCtrl_);
		upperbox_.append(*blendCombo);
		upperbox_.append(*blendSizeCtrl);
		upperbox_.append(*motionCheck);
		lowerbox_.append(typeCombo_);
		lowerbox_.append(vibWidget_);

		GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &PolygonCornerCtrl::OnClick));
		typeCombo_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerCtrl::OnComboChanged));
	}
	virtual ~PolygonCornerCtrl() {};
protected:
	void OnClick() { peer_->SetCornerEnable(GetCheck().get_active()); };
	void OnComboChanged() { vibWidget_.set_sensitive(peer_->GetCornerType()==Polygon::corner_type_solid); }

    PolygonCornerAngleCtrl angleCtrl_;
	PolygonCornerTypeCombo typeCombo_;
	OscillatorWidget vibWidget_;
	Gtk::Box mainbox_;
	Gtk::Box upperbox_;
	Gtk::Box lowerbox_;
    PolygonPtr peer_;
};

class PolygonVerticesCtrl : public Gtk::Frame
{
public:
	PolygonVerticesCtrl(PolygonPtr peer) : Gtk::Frame("vertices"), list_(peer->GetPoints()), peer_(peer)
	{
		set_child(list_);
		list_.SignalChanged().connect(sigc::mem_fun(*this, &PolygonVerticesCtrl::OnChanged));
	}
	virtual ~PolygonVerticesCtrl() {};

protected:
	void OnChanged() { peer_->SetPoints(list_.GetPoints()); }

	PointListWidget list_;
	PolygonPtr peer_;
};

class PolygonPropertiesCtrl : public Gtk::Box
{
public:
	PolygonPropertiesCtrl(PolygonPtr peer) : Gtk::Box(Gtk::Orientation::HORIZONTAL), box_(Gtk::Orientation::VERTICAL)
	{
		append(box_);
		box_.set_hexpand();
		box_.append(*Gtk::make_managed<PolygonJoinTypeCtrl>(peer));
		box_.append(*Gtk::make_managed<PolygonCornerCtrl>(peer));
		append(*Gtk::make_managed<PolygonVerticesCtrl>(peer));
		auto patternPreview = Gtk::make_managed<PatternPreview>(peer);
		patternPreview->set_size_request(200,-1);
		append(*patternPreview);
	}
	virtual ~PolygonPropertiesCtrl() {};
protected:
	Gtk::Box box_;
};


PolygonWidget::PolygonWidget(PolygonPtr peer)
{
	append_page(*Gtk::make_managed<PolygonPropertiesCtrl>(peer), "properties");
	append_page(*Gtk::make_managed<StrokeProfileWidget>(peer->GetStroke()), "stroke");
	append_page(*Gtk::make_managed<StrokeFillWidget>(peer->GetStroke()), "fill");
	append_page(*Gtk::make_managed<StrokeMotionWidget>(peer->GetStroke()),"motion");
	append_page(*Gtk::make_managed<StrokeDottedWidget>(peer->GetStroke()),"dots");
	append_page(*Gtk::make_managed<ClosedPatternFillTextureWidget>(peer),"texture");
}

} // namespace graphics
} // namespace latero

