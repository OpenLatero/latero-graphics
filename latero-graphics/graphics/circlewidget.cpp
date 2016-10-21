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
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
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
		add(widget_);
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &CircleCenterCtrl::OnChanged));
	}
	virtual ~CircleCenterCtrl() {};
protected:
	void OnChanged() { peer_->SetCenter(widget_.GetValue()); }
	CirclePtr peer_;
	PointWidget widget_;
};


class CircleRadiusCtrl : public Gtk::VBox
{
public:
	CircleRadiusCtrl(CirclePtr peer) : 
		adj_(peer->GetRadius(), 0.1, 100.0), peer_(peer)
	{
		add(*manage(new latero::graphics::gtk::HNumWidget("radius", adj_, 1, "mm")));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &CircleRadiusCtrl::OnChanged));
	}
	virtual ~CircleRadiusCtrl() {};
protected:
	void OnChanged() { peer_->SetRadius(adj_.get_value()); }
	Gtk::Adjustment adj_;	
	CirclePtr peer_;
};


class CirclePropsWidget : public Gtk::HBox
{
public:
	CirclePropsWidget(CirclePtr peer)
	{
		Gtk::Box *box = manage(new Gtk::VBox);
		box->pack_start(*manage(new CircleCenterCtrl(peer)),Gtk::PACK_SHRINK);
		box->pack_start(*manage(new CircleRadiusCtrl(peer)),Gtk::PACK_SHRINK);

		pack_start(*box);
		pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);

	}
	virtual ~CirclePropsWidget() {};
};


CircleWidget::CircleWidget(CirclePtr peer)
{
	append_page(*manage(new CirclePropsWidget(peer)),"properties");
	append_page(*manage(new StrokeProfileWidget(peer->GetStroke())),"stroke");
	append_page(*manage(new StrokeFillWidget(peer->GetStroke())),"fill");
	append_page(*manage(new StrokeMotionWidget(peer->GetStroke())),"motion");
	append_page(*manage(new StrokeDottedWidget(peer->GetStroke())),"dots");
	append_page(*manage(new ClosedPatternFillTextureWidget(peer)),"texture");
}

} // namespace graphics
} // namespace latero

