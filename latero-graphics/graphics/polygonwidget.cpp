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

class PolygonJoinTypeDropDown : public Gtk::Box
{
public:
	PolygonJoinTypeDropDown(PolygonPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL),
		list_(Gtk::StringList::create({})),
		dropDown_(list_),
		peer_(peer)
	{
		for (const auto& op : peer->GetJoinTypes())
			list_->append(op.label);
		Glib::ustring target = peer->GetJoinType().label;
		for (guint i = 0; i < list_->get_n_items(); ++i)
			if (list_->get_string(i) == target) { dropDown_.set_selected(i); break; }
		dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &PolygonJoinTypeDropDown::OnChange));
		append(dropDown_);
	};
	virtual ~PolygonJoinTypeDropDown() {};
	sigc::signal<void()>& SignalChanged() { return signalChanged_; };
private:
	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	sigc::signal<void()> signalChanged_;
	void OnChange() { peer_->SetJoinType(std::string(list_->get_string(dropDown_.get_selected()))); signalChanged_(); };
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
		box_(Gtk::Orientation::HORIZONTAL), Gtk::Frame("join"), dropDown_(peer), offsetCtrl_(peer), dropDownFrame_("type"), peer_(peer)
	{
		set_child(box_);
		box_.append(dropDownFrame_);
		dropDownFrame_.set_child(dropDown_);
		offsetCtrl_.set_hexpand();
		box_.append(offsetCtrl_);
		OnChange();
		dropDown_.SignalChanged().connect(sigc::mem_fun(*this, &PolygonJoinTypeCtrl::OnChange));
	}
	virtual ~PolygonJoinTypeCtrl() {}
protected:
	void OnChange() { offsetCtrl_.set_sensitive(peer_->GetJoinType()==Polygon::join_type_round); }

	Gtk::Box box_;
	PolygonJoinTypeDropDown dropDown_;
	PolygonRoundingOffsetCtrl offsetCtrl_;
	Gtk::Frame dropDownFrame_;
	PolygonPtr peer_;
};





class PolygonCornerSpanDropDown : public Gtk::Box
{
public:
	PolygonCornerSpanDropDown(PolygonPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL),
		list_(Gtk::StringList::create({})),
		dropDown_(list_),
		peer_(peer)
	{
		for (const auto& op : peer->GetCornerSpans())
			list_->append(op.label);
		Glib::ustring target = peer->GetCornerSpan().label;
		for (guint i = 0; i < list_->get_n_items(); ++i)
			if (list_->get_string(i) == target) { dropDown_.set_selected(i); break; }
		dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerSpanDropDown::OnChange));
		append(dropDown_);
	};
	virtual ~PolygonCornerSpanDropDown() {};
	sigc::signal<void()>& signal_changed() { return signalChanged_; };
private:
	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	sigc::signal<void()> signalChanged_;
	void OnChange() { peer_->SetCornerSpan(std::string(list_->get_string(dropDown_.get_selected()))); signalChanged_(); };
	PolygonPtr peer_;
};

class PolygonCornerBlendDropDown : public Gtk::Frame
{
public:
	PolygonCornerBlendDropDown(PolygonPtr peer) : Gtk::Frame("blend"), peer_(peer)
	{
		opsList_ = Gtk::StringList::create({});
		Polygon::CornerBlendSet ops = peer->GetCornerBlends();
		for (unsigned int i=0; i<ops.size(); ++i)
			opsList_->append(ops[i].label);
		opsDropDown_ = Gtk::make_managed<Gtk::DropDown>(opsList_);
		Glib::ustring target = peer->GetCornerBlend().label;
		for (guint i = 0; i < opsList_->get_n_items(); ++i)
			if (opsList_->get_string(i) == target) { opsDropDown_->set_selected(i); break; }
		opsDropDown_->property_selected().signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerBlendDropDown::OnChange));
		set_child(*opsDropDown_);
	};
	virtual ~PolygonCornerBlendDropDown() {};
private:
	Glib::RefPtr<Gtk::StringList> opsList_;
	Gtk::DropDown* opsDropDown_;
	void OnChange() { peer_->SetCornerBlend(std::string(opsList_->get_string(opsDropDown_->get_selected()))); };
	PolygonPtr peer_;
};

class PolygonCornerTypeDropDown : public Gtk::Frame
{
public:
	PolygonCornerTypeDropDown(PolygonPtr peer) : Gtk::Frame("type"), peer_(peer)
	{
		opsList_ = Gtk::StringList::create({});
		Polygon::CornerTypeSet ops = peer->GetCornerTypes();
		for (unsigned int i=0; i<ops.size(); ++i)
			opsList_->append(ops[i].label);
		opsDropDown_ = Gtk::make_managed<Gtk::DropDown>(opsList_);
		Glib::ustring target = peer->GetCornerType().label;
		for (guint i = 0; i < opsList_->get_n_items(); ++i)
			if (opsList_->get_string(i) == target) { opsDropDown_->set_selected(i); break; }
		opsDropDown_->property_selected().signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerTypeDropDown::OnChange));
		set_child(*opsDropDown_);
	};
	virtual ~PolygonCornerTypeDropDown() {};
	sigc::signal<void()>& signal_changed() { return signalChanged_; };
private:
	Glib::RefPtr<Gtk::StringList> opsList_;
	Gtk::DropDown* opsDropDown_;
	sigc::signal<void()> signalChanged_;
	void OnChange() { peer_->SetCornerType(std::string(opsList_->get_string(opsDropDown_->get_selected()))); signalChanged_(); };
	PolygonPtr peer_;
};


class PolygonCornerAngleCtrl : public Gtk::Frame
{
public:
	PolygonCornerAngleCtrl(PolygonPtr peer) : 
		Gtk::Frame("angle"), 
		adj_(Gtk::Adjustment::create(peer->GetCornerUserAngle(), 0, 180)),
		angleCtrl_(adj_),
        dropDown_(peer), 
        peer_(peer)
	{
		angleCtrl_.set_sensitive(peer->GetCornerSpan()==Polygon::corner_span_user );
		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
		box->append(dropDown_);
		angleCtrl_.set_hexpand();
		box->append(angleCtrl_);
		set_child(*box);
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &PolygonCornerAngleCtrl::OnChanged));
		dropDown_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerAngleCtrl::OnDropDownChanged));
	}
	virtual ~PolygonCornerAngleCtrl() {};
protected:
	void OnChanged() { peer_->SetCornerUserAngle(adj_->get_value()); }
	void OnDropDownChanged() { angleCtrl_.set_sensitive(peer_->GetCornerSpan()==Polygon::corner_span_user); }

    Glib::RefPtr<Gtk::Adjustment> adj_;	
	Gtk::SpinButton angleCtrl_;
	PolygonCornerSpanDropDown dropDown_;
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
		angleCtrl_(peer), typeDropDown_(peer), vibWidget_(peer->GetCornerOscillator()), peer_(peer)
	{
		vibWidget_.set_sensitive(peer_->GetCornerType()==Polygon::corner_type_solid);

		auto blendDropDown = Gtk::make_managed<PolygonCornerBlendDropDown>(peer);
		auto blendSizeCtrl = Gtk::make_managed<PolygonCornerBlendSizeCtrl>(peer);
		auto motionCheck = Gtk::make_managed<PolygonCornerMotionCheck>(peer);

		mainbox_.set_hexpand();
		upperbox_.set_vexpand();
		lowerbox_.set_vexpand();
		angleCtrl_.set_hexpand();
		blendDropDown->set_hexpand();
		blendSizeCtrl->set_hexpand();
		motionCheck->set_hexpand();
		typeDropDown_.set_hexpand();
		vibWidget_.set_hexpand();

		GetBox().append(mainbox_);
		mainbox_.append(upperbox_);
		mainbox_.append(lowerbox_);
		upperbox_.append(angleCtrl_);
		upperbox_.append(*blendDropDown);
		upperbox_.append(*blendSizeCtrl);
		upperbox_.append(*motionCheck);
		lowerbox_.append(typeDropDown_);
		lowerbox_.append(vibWidget_);

		GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &PolygonCornerCtrl::OnClick));
		typeDropDown_.signal_changed().connect(sigc::mem_fun(*this, &PolygonCornerCtrl::OnDropDownChanged));
	}
	virtual ~PolygonCornerCtrl() {};
protected:
	void OnClick() { peer_->SetCornerEnable(GetCheck().get_active()); };
	void OnDropDownChanged() { vibWidget_.set_sensitive(peer_->GetCornerType()==Polygon::corner_type_solid); }

    PolygonCornerAngleCtrl angleCtrl_;
	PolygonCornerTypeDropDown typeDropDown_;
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

