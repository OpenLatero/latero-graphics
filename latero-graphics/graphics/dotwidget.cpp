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

#include "dotwidget.h"
#include "dot.h"
#include "../gtk/numwidget.h"
#include "../ridgewidget.h"
#include "../pointwidget.h"
#include "../oscillatorwidget.h"

namespace latero {
namespace graphics {

DotRadiusCtrl::DotRadiusCtrl(DotPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	peer_(peer),
	adj_(Gtk::Adjustment::create(peer->GetDotRadius(), 0.01, 20.0))
{
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DotRadiusCtrl::OnChanged));
	append(*Gtk::make_managed<gtk::HNumWidget>("radius",adj_,1,"mm"));
}
void DotRadiusCtrl::OnChanged() { peer_->SetDotRadius(adj_->get_value()); }


DotHeightCtrl::DotHeightCtrl(DotPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	peer_(peer),
	adj_(Gtk::Adjustment::create(peer->GetHeight()*100, 1, 100))
{
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DotHeightCtrl::OnChanged));
	append(*Gtk::make_managed<gtk::HNumWidget>("height",adj_,0,"%"));
}
void DotHeightCtrl::OnChanged() { peer_->SetHeight(adj_->get_value()/100); };


class DotPosCtrl : public Gtk::Frame
{
public:
	DotPosCtrl(DotPtr peer) :
		Gtk::Frame("center"),
		peer_(peer),
		widget_(peer->GetPos(),0,peer->Dev()->GetSurfaceWidth(),0,peer->Dev()->GetSurfaceHeight())
	{
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &DotPosCtrl::OnChanged));
		set_child(widget_);
	}
	virtual ~DotPosCtrl() {}
protected:
	void OnChanged()  { peer_->SetPos(widget_.GetValue()); };
	DotPtr peer_;
	PointWidget widget_;
};



DotWidget::DotWidget(DotPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL)
{
	auto grid = Gtk::make_managed<Gtk::Grid>();
	grid->attach(*Gtk::make_managed<DotPosCtrl>(peer),0,0,3,1);
	grid->attach(*Gtk::make_managed<DotRadiusCtrl>(peer),0,1,1,1);
	grid->attach(*Gtk::make_managed<DotHeightCtrl>(peer),1,1,1,1);
	grid->attach(*Gtk::make_managed<RidgeEdgeWidthScale>(peer->GetProfile()),2,1,1,1);
	grid->attach(*Gtk::make_managed<RidgeTextureCtrl>(peer->GetProfile()),0,2,3,1);
	grid->attach(*Gtk::make_managed<OscillatorWidget>(peer->GetOscillator()),0,3,3,1);

	append(*grid);
	grid->set_hexpand();
	append(*Gtk::make_managed<RidgeGraph>(peer->GetProfile(),300));
}

} // namespace graphics
} // namespace latero

