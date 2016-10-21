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

#include "dotswidget.h"
#include "dots.h"
#include "../gtk/numwidget.h"
#include "../ridgewidget.h"
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include "../pointwidget.h"
#include "../oscillatorwidget.h"
#include "../pointlistwidget.h"

namespace latero {
namespace graphics { 

class DotsPointsWidget : public Gtk::Frame
{
public:
	DotsPointsWidget(DotsPtr peer) : 
		Gtk::Frame("points"), widget_(peer->GetPoints()), peer_(peer)
	{
		add(widget_);
		widget_.SignalChanged().connect(sigc::mem_fun(*this, &DotsPointsWidget::OnChanged));
	}
	~DotsPointsWidget() {}
protected:
	void OnChanged() { peer_->SetPoints(widget_.GetPoints()); }
	PointListWidget widget_;
	DotsPtr peer_;
};

DotsRadiusCtrl::DotsRadiusCtrl(DotsPtr peer) :
	peer_(peer),
	adj_(peer->GetDotRadius(), 0.01, 20.0)
{
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &DotsRadiusCtrl::OnChanged));
	add(*manage(new gtk::HNumWidget("radius",adj_,1,"mm")));
}
void DotsRadiusCtrl::OnChanged() { peer_->SetDotRadius(adj_.get_value()); }


DotsHeightCtrl::DotsHeightCtrl(DotsPtr peer) :
	peer_(peer),
	adj_(peer->GetHeight()*100, 1, 100)
{
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &DotsHeightCtrl::OnChanged));
	add(*manage(new gtk::HNumWidget("height",adj_,0,"%")));
}
void DotsHeightCtrl::OnChanged() { peer_->SetHeight(adj_.get_value()/100); };


DotsWidget::DotsWidget(DotsPtr peer)
{
	Gtk::Table *table = manage(new Gtk::Table(3,4));
	table->attach(*manage(new DotsRadiusCtrl(peer)), 			0,2, 0,1);
	table->attach(*manage(new DotsHeightCtrl(peer)), 			0,1, 1,2);
	table->attach(*manage(new RidgeEdgeWidthScale(peer->GetProfile())), 	1,2, 1,2);
	table->attach(*manage(new RidgeTextureCtrl(peer->GetProfile())), 	0,2, 2,3);
	table->attach(*manage(new OscillatorWidget(peer->GetOscillator())), 	0,2, 3,4);
	table->attach(*manage(new DotsPointsWidget(peer)), 	2,3, 0,4);

	pack_start(*table);
	pack_start(*manage(new RidgeGraph(peer->GetProfile(),300)), Gtk::PACK_SHRINK);
}

} // namespace graphics
} // namespace latero

