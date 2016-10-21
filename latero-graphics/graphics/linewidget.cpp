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

#include "linewidget.h"
#include <iostream>
#include "line.h"
#include "strokewidget.h"
#include "../pointwidget.h"
#include "dotpattern.h"
#include <gtkmm/frame.h>
#include "../gtk/numwidget.h"
#include "patternpreview.h"

namespace latero {
namespace graphics { 

class LineStartCtrl : public Gtk::Frame
{
public:
	LineStartCtrl(LinePtr peer) :
		Gtk::Frame("start point"),
		peer_(peer),
		widget_(peer->GetStartPoint(),0,peer->Dev()->GetSurfaceWidth(),0,peer->Dev()->GetSurfaceHeight())
	{
		add(widget_);
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &LineStartCtrl::OnChanged));
	}
	virtual ~LineStartCtrl() {};
protected:
	void OnChanged() { peer_->SetStartPoint(widget_.GetValue()); }
	LinePtr peer_;
	PointWidget widget_;
};


class LineEndCtrl : public Gtk::Frame
{
public:
	LineEndCtrl(LinePtr peer) :
		Gtk::Frame("end point"),
		peer_(peer),
		widget_(peer->GetEndPoint(),0,peer->Dev()->GetSurfaceWidth(),0,peer->Dev()->GetSurfaceHeight())
	{
		add(widget_);
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &LineEndCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetEndPoint(widget_.GetValue()); }
	LinePtr peer_;
	PointWidget widget_;
};

class LinePropertiesCtrl : public Gtk::HBox
{
public:
	LinePropertiesCtrl(LinePtr peer)
	{
		Gtk::Box *box = manage(new Gtk::VBox);
		box->pack_start(*manage(new LineStartCtrl(peer)), Gtk::PACK_SHRINK);
		box->pack_start(*manage(new LineEndCtrl(peer)), Gtk::PACK_SHRINK);

		pack_start(*box);
		pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);
	}
	virtual ~LinePropertiesCtrl() {};
};

LineWidget::LineWidget(LinePtr peer)
{
	append_page(*manage(new LinePropertiesCtrl(peer)), "properties");
	append_page(*manage(new StrokeProfileWidget(peer->GetStroke())), "stroke");
	append_page(*manage(new StrokeFillWidget(peer->GetStroke())), "fill");
	append_page(*manage(new StrokeMotionWidget(peer->GetStroke())),"motion");
	append_page(*manage(new StrokeDottedWidget(peer->GetStroke())),"dots");
}


} // namespace graphics
} // namespace latero

