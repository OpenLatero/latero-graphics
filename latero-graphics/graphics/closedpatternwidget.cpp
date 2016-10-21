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
#include "closedpattern.h"
#include "../gtk/numwidget.h"

namespace latero {
namespace graphics { 

class ClosedPatternFillTextureGapCtrl : public Gtk::VBox
{
public:
	ClosedPatternFillTextureGapCtrl(ClosedPatternPtr peer) : 
		adj_(peer->GetFillTextureGap(), -1, 50), peer_(peer)
	{
		add(*manage(new latero::graphics::gtk::HNumWidget("gap", adj_, 1, "mm")));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureGapCtrl::OnChanged));
	}
	virtual ~ClosedPatternFillTextureGapCtrl() {};
protected:
	void OnChanged() { peer_->SetFillTextureGap(adj_.get_value()); }
	Gtk::Adjustment adj_;	
	ClosedPatternPtr peer_;
};

class ClosedPatternFillTextureEdgeSizeCtrl : public Gtk::VBox
{
public:
	ClosedPatternFillTextureEdgeSizeCtrl(ClosedPatternPtr peer) : 
		adj_(peer->GetFillTextureEdgeSize(), 0, 50), peer_(peer)
	{
		add(*manage(new latero::graphics::gtk::HNumWidget("edge size", adj_, 1, "mm")));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureEdgeSizeCtrl::OnChanged));
	}
	virtual ~ClosedPatternFillTextureEdgeSizeCtrl() {};
protected:
	void OnChanged() { peer_->SetFillTextureEdgeSize(adj_.get_value()); }
	Gtk::Adjustment adj_;	
	ClosedPatternPtr peer_;
};





ClosedPatternFillTextureWidget::ClosedPatternFillTextureWidget(ClosedPatternPtr peer) :
	gtk::CheckFrame(peer->GetFillTextureEnable()), txWidget_(peer->GetFillTexture()), peer_(peer)
{
	set_shadow_type(Gtk::SHADOW_NONE);

	Gtk::VBox *vbox = manage(new Gtk::VBox);
	GetBox().pack_start(*vbox);

	Gtk::HBox *hbox = manage(new Gtk::HBox);
	hbox->pack_start(*manage(new ClosedPatternFillTextureGapCtrl(peer)));	
	hbox->pack_start(*manage(new ClosedPatternFillTextureEdgeSizeCtrl(peer)));

	vbox->pack_start(*hbox, Gtk::PACK_SHRINK);
	vbox->pack_start(txWidget_);

	txWidget_.SignalTextureChanged().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureWidget::OnTextureChanged));;
	GetCheck().signal_clicked().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureWidget::OnCheck));
}
void ClosedPatternFillTextureWidget::OnCheck() { peer_->SetFillTextureEnable(GetCheck().get_active()); }
void ClosedPatternFillTextureWidget::OnTextureChanged() { peer_->SetFillTexture(txWidget_.GetTexture()); }


} // namespace graphics
} // namespace latero

