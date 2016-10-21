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
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include "../pointwidget.h"
#include "../oscillatorwidget.h"
#include "../pointlistwidget.h"
#include "dotswidget.h"

namespace latero {
namespace graphics { 

DotPatternMinSpacingCtrl::DotPatternMinSpacingCtrl(DotPatternPtr peer) :
    adj_(peer->GetMinSpacing(), 0, 50.0), peer_(peer)
{
	adj_.signal_value_changed().connect(sigc::mem_fun(*this, &DotPatternMinSpacingCtrl::OnChanged));
	add(*manage(new gtk::HNumWidget("minimal spacing",adj_,1,"mm")));
}
void DotPatternMinSpacingCtrl::OnChanged() { peer_->SetMinSpacing(adj_.get_value()); }

DotPatternWidget::DotPatternWidget(DotPatternPtr peer)
{
	Gtk::Table *table = manage(new Gtk::Table(2,4));
	table->attach(*manage(new DotPatternMinSpacingCtrl(peer)), 		0,1, 0,1);
	table->attach(*manage(new DotsHeightCtrl(peer)), 			0,1, 1,2);
	table->attach(*manage(new RidgeEdgeWidthScale(peer->GetProfile())), 	1,2, 1,2);
	table->attach(*manage(new RidgeTextureCtrl(peer->GetProfile())), 	0,2, 2,3);
	table->attach(*manage(new OscillatorWidget(peer->GetOscillator())), 	0,2, 3,4);
	pack_start(*table);
	pack_start(*manage(new RidgeGraph(peer->GetProfile(),300)), Gtk::PACK_SHRINK);
}

} // namespace graphics
} // namespace latero

