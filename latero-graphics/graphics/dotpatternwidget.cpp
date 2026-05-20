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

#include "dotpatternwidget.h"
#include "dotpattern.h"
#include "../gtk/numwidget.h"
#include "../ridgewidget.h"
#include "../pointwidget.h"
#include "../oscillatorwidget.h"
#include "../pointlistwidget.h"
#include "dotswidget.h"

namespace latero::graphics {

DotPatternMinSpacingCtrl::DotPatternMinSpacingCtrl(DotPatternPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetMinSpacing(), 0, 50.0)), peer_(peer)
{
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DotPatternMinSpacingCtrl::OnChanged));
	append(*Gtk::make_managed<gtk::HNumWidget>("minimal spacing",adj_,1,"mm"));
}
void DotPatternMinSpacingCtrl::OnChanged() { peer_->SetMinSpacing(adj_->get_value()); }

DotPatternWidget::DotPatternWidget(DotPatternPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL)
{
	auto grid = Gtk::make_managed<Gtk::Grid>();
	grid->attach(*Gtk::make_managed<DotPatternMinSpacingCtrl>(peer),0,0,1,1);
	grid->attach(*Gtk::make_managed<DotsHeightCtrl>(peer),0,1,1,1);
	grid->attach(*Gtk::make_managed<RidgeEdgeWidthScale>(peer->GetProfile()),1,1,1,1);
	grid->attach(*Gtk::make_managed<RidgeTextureCtrl>(peer->GetProfile()),0,2,2,1);
	grid->attach(*Gtk::make_managed<OscillatorWidget>(peer->GetOscillator()),0,3,2,1);
	append(*grid);
	grid->set_hexpand();
	append(*Gtk::make_managed<RidgeGraph>(peer->GetProfile(),300));
}

} // namespace

