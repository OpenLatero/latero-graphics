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

#include "ridgewidget.h"
#include <iostream>
#include <gtkmm/expander.h>
#include "gtk/numwidget.h"
#include "ridge.h"

namespace latero {
namespace graphics { 

RidgeEdgeWidthScale::RidgeEdgeWidthScale(RidgePtr peer) :
	peer_(peer),
	adj_(peer->GetEdgeWidth(), Ridge::edgeWidth_min, 10, 10, 50)
{
	add(*manage(new gtk::HNumWidget("edge width", adj_, 2, "mm")));
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &RidgeEdgeWidthScale::OnChange));
}
void RidgeEdgeWidthScale::OnChange() { peer_->SetEdgeWidth(adj_.get_value()); }




class RidgeTxAmpScale : public Gtk::VBox
{
public:
	RidgeTxAmpScale(RidgePtr peer) :
		peer_(peer), adj_(100*peer->GetTxAmp(), 0, 100, 10, 50)
	{
		add(*manage(new gtk::HNumWidget(adj_, 0, "%")));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &RidgeTxAmpScale::OnChange));
	}
	virtual ~RidgeTxAmpScale() {};
private:
	void OnChange() { peer_->SetTxAmp(adj_.get_value()/100); };
	RidgePtr peer_;
	Gtk::Adjustment adj_;
};

class RidgeTxNbCyclesScale : public Gtk::VBox
{
public:
	RidgeTxNbCyclesScale(RidgePtr peer) :
		peer_(peer), adj_(peer->GetTxNbCycles(),1,20.0)
	{
		add(*manage(new gtk::HNumWidget(adj_, 0)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &RidgeTxNbCyclesScale::OnChange));
	}
	virtual ~RidgeTxNbCyclesScale() {};
private:
	void OnChange() { peer_->SetTxNbCycles((uint)adj_.get_value()); };
	RidgePtr peer_;
	Gtk::Adjustment adj_;
};


RidgeTextureCtrl::RidgeTextureCtrl(RidgePtr peer) :
	gtk::CheckFrame(peer->GetTxEnable(), "texture"), peer_(peer)
{
	GetBox().pack_start(*manage(new RidgeTxAmpScale(peer)));
	GetBox().pack_start(*manage(new RidgeTxNbCyclesScale(peer)));
	GetCheck().signal_clicked().connect(sigc::mem_fun(*this, &RidgeTextureCtrl::OnClick));
}
void RidgeTextureCtrl::OnClick() { peer_->SetTxEnable(GetCheck().get_active()); };



RidgeControls::RidgeControls(RidgePtr peer) :
	peer_(peer),
	edgeWidthScale_(peer),
	txCtrl_(peer)
{
	set_size_request(300,-1);
	pack_start(edgeWidthScale_);
	pack_start(txCtrl_);
}


///////////////////////////////////////

RidgeWidget::RidgeWidget(RidgePtr peer) :
	controls_(peer),
    peer_(peer)
{
	graph_ = manage(new RidgeGraph(peer));
	pack_start(controls_);
	pack_start(*graph_);
	graph_->Refresh();
}

} // namespace graphics
} // namespace latero

