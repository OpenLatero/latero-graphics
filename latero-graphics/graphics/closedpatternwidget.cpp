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

class ClosedPatternFillTextureGapCtrl : public Gtk::Box
{
public:
	ClosedPatternFillTextureGapCtrl(ClosedPatternPtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetFillTextureGap(), -1, 50)), peer_(peer)
	{
		append(*Gtk::manage(new latero::graphics::gtk::HNumWidget("gap", adj_, 1, "mm")));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureGapCtrl::OnChanged));
	}
	virtual ~ClosedPatternFillTextureGapCtrl() {};
protected:
	void OnChanged() { peer_->SetFillTextureGap(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	ClosedPatternPtr peer_;
};

class ClosedPatternFillTextureEdgeSizeCtrl : public Gtk::Box
{
public:
	ClosedPatternFillTextureEdgeSizeCtrl(ClosedPatternPtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetFillTextureEdgeSize(), 0, 50)), peer_(peer)
	{
		append(*Gtk::manage(new latero::graphics::gtk::HNumWidget("edge size", adj_, 1, "mm")));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureEdgeSizeCtrl::OnChanged));
	}
	virtual ~ClosedPatternFillTextureEdgeSizeCtrl() {};
protected:
	void OnChanged() { peer_->SetFillTextureEdgeSize(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;	
	ClosedPatternPtr peer_;
};





ClosedPatternFillTextureWidget::ClosedPatternFillTextureWidget(ClosedPatternPtr peer) :
	gtk::CheckFrame(peer->GetFillTextureEnable()), txWidget_(peer->GetFillTexture()), peer_(peer)
{
	auto vbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
	auto gapCtrl = Gtk::manage(new ClosedPatternFillTextureGapCtrl(peer));
	auto edgeSizeCtrl = Gtk::manage(new ClosedPatternFillTextureEdgeSizeCtrl(peer));

	vbox->set_hexpand();
	gapCtrl->set_hexpand();
	edgeSizeCtrl->set_hexpand();
	txWidget_.set_vexpand();

	GetBox().append(*vbox);
	auto hbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	hbox->append(*gapCtrl);	
	hbox->append(*edgeSizeCtrl);
	vbox->append(*hbox);
	vbox->append(txWidget_);

	txWidget_.SignalTextureChanged().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureWidget::OnTextureChanged));;
	GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureWidget::OnCheck));
}
void ClosedPatternFillTextureWidget::OnCheck() { peer_->SetFillTextureEnable(GetCheck().get_active()); }
void ClosedPatternFillTextureWidget::OnTextureChanged() { peer_->SetFillTexture(txWidget_.GetTexture()); }


} // namespace graphics
} // namespace latero

