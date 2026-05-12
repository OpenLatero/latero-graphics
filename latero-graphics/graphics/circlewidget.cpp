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

#include "closedpatternwidget.h"
#include "circlewidget.h"
#include <iostream>
#include "circle.h"
#include "../gtk/numwidget.h"
#include "strokewidget.h"
#include "../pointwidget.h"
#include "patternpreview.h"


namespace latero {
namespace graphics { 

class CircleCenterCtrl : public Gtk::Frame
{
public:
	CircleCenterCtrl(CirclePtr peer) :
		Gtk::Frame("center"),
		peer_(peer),
		widget_(peer->GetCenter(),0,peer->Dev()->GetSurfaceWidth(),0,peer->Dev()->GetSurfaceHeight())
	{
		set_child(widget_);
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &CircleCenterCtrl::OnChanged));
	}
	virtual ~CircleCenterCtrl() {};
protected:
	void OnChanged() { peer_->SetCenter(widget_.GetValue()); }
	CirclePtr peer_;
	PointWidget widget_;
};


class CircleRadiusCtrl : public Gtk::Box
{
public:
	CircleRadiusCtrl(CirclePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetRadius(), 0.1, 100.0)), peer_(peer)
	{
		append(*Gtk::make_managed<latero::graphics::gtk::HNumWidget>("radius", adj_, 1, "mm"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &CircleRadiusCtrl::OnChanged));
	}
	virtual ~CircleRadiusCtrl() {};
protected:
	void OnChanged() { peer_->SetRadius(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;	
	CirclePtr peer_;
};


class CirclePropsWidget : public Gtk::Box
{
public:
	CirclePropsWidget(CirclePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL)
	{
		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
		box->append(*Gtk::make_managed<CircleCenterCtrl>(peer));
		box->append(*Gtk::make_managed<CircleRadiusCtrl>(peer));

		box->set_hexpand();

		append(*box);
		append(*Gtk::make_managed<PatternPreview>(peer));

	}
	virtual ~CirclePropsWidget() {};
};


CircleWidget::CircleWidget(CirclePtr peer)
{
	append_page(*Gtk::make_managed<CirclePropsWidget>(peer),"properties");
	append_page(*Gtk::make_managed<StrokeProfileWidget>(peer->GetStroke()),"stroke");
	append_page(*Gtk::make_managed<StrokeFillWidget>(peer->GetStroke()),"fill");
	append_page(*Gtk::make_managed<StrokeMotionWidget>(peer->GetStroke()),"motion");
	append_page(*Gtk::make_managed<StrokeDottedWidget>(peer->GetStroke()),"dots");
	append_page(*Gtk::make_managed<ClosedPatternFillTextureWidget>(peer),"texture");
}

} // namespace graphics
} // namespace latero

