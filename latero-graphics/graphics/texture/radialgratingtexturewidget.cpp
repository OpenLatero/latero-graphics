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

#include "radialgratingtexturewidget.h"
#include <gtkmm/spinbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/table.h>
#include "radialgratingtexture.h"

namespace latero {
namespace graphics { 

void RadialGratingTextureWidget::Create()
{
	Gtk::HBox *checks = manage(new Gtk::HBox);
	checks->pack_start(*CreateVibCheck());
	checks->pack_start(*CreateTDCentricCheck());

	Gtk::Table *table = manage(new Gtk::Table(2,3));
	table->attach(*CreateSeedWidget(),0,2,0,1);
	table->attach(*CreateRidgeSizeWidget(),0,1,1,2);
	table->attach(*CreateGapSizeWidget(),1,2,1,2);
	table->attach(*checks,0,1,2,3);
	table->attach(*CreateGratingVelocityWidget(),1,2,2,3);
	SetContent(table);
}

RadialGratingTextureAdvancedWidget::RadialGratingTextureAdvancedWidget(RadialGratingTexturePtr peer) :
	GratingTextureWidgetSet(peer),
	peer_(peer)
{
	Gtk::VBox *vbox = manage(new Gtk::VBox);
	vbox->pack_start(seedCtrl_);
	vbox->pack_start(*manage(new GratingPitchWidget(peer->GetGrating())));
	vbox->pack_start(*manage(new GratingVelocityWidget(peer->GetGrating())));
	vbox->pack_start(*manage(new GratingAdvancedButton(peer->GetGrating())),Gtk::PACK_SHRINK);
	vbox->pack_start(tdCentricCtrl_);
	vbox->pack_start(vibCtrl_);
	vbox->pack_start(*manage(new TextureMotionCtrl(peer)));

	Gtk::VBox *lbox = manage(new Gtk::VBox);
	lbox->pack_start(invertCtrl_, Gtk::PACK_SHRINK);
	lbox->pack_start(ampCtrl_);

	Gtk::HBox *hbox = manage(new Gtk::HBox);
	add(*hbox);
	hbox->pack_start(*lbox, Gtk::PACK_SHRINK);
	hbox->pack_start(*vbox);
	hbox->pack_start(preview_, Gtk::PACK_SHRINK);

	show_all_children();
}

} // namespace graphics
} // namespace latero
