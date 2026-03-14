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
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include "../pointwidget.h"
#include "../oscillatorwidget.h"

namespace latero {
namespace graphics { 

DotRadiusCtrl::DotRadiusCtrl(DotPtr peer) :
	peer_(peer),
	adj_(Gtk::Adjustment::create(peer->GetDotRadius(), 0.01, 20.0))
{
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DotRadiusCtrl::OnChanged));
	add(*Gtk::manage(new gtk::HNumWidget("radius",adj_,1,"mm")));
}
void DotRadiusCtrl::OnChanged() { peer_->SetDotRadius(adj_->get_value()); }


DotHeightCtrl::DotHeightCtrl(DotPtr peer) :
	peer_(peer),
	adj_(Gtk::Adjustment::create(peer->GetHeight()*100, 1, 100))
{
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DotHeightCtrl::OnChanged));
	add(*Gtk::manage(new gtk::HNumWidget("height",adj_,0,"%")));
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
		add(widget_);
	}
	virtual ~DotPosCtrl() {}
protected:
	void OnChanged()  { peer_->SetPos(widget_.GetValue()); };
	DotPtr peer_;
	PointWidget widget_;
};



DotWidget::DotWidget(DotPtr peer)
{
	auto grid = Gtk::manage(new Gtk::Grid());
	grid->attach(*Gtk::manage(new DotPosCtrl(peer)),0,0,3,1);
	grid->attach(*Gtk::manage(new DotRadiusCtrl(peer)),0,1,1,1);
	grid->attach(*Gtk::manage(new DotHeightCtrl(peer)),1,1,1,1);
	grid->attach(*Gtk::manage(new RidgeEdgeWidthScale(peer->GetProfile())),2,1,1,1);
	grid->attach(*Gtk::manage(new RidgeTextureCtrl(peer->GetProfile())),0,2,3,1);
	grid->attach(*Gtk::manage(new OscillatorWidget(peer->GetOscillator())),0,3,3,1);

	pack_start(*grid);
	pack_start(*Gtk::manage(new RidgeGraph(peer->GetProfile(),300)), Gtk::PACK_SHRINK);
}

} // namespace graphics
} // namespace latero

