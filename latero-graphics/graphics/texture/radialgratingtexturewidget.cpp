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
	Gtk::HBox *checks = Gtk::manage(new Gtk::HBox);
	checks->pack_start(*CreateVibCheck());
	checks->pack_start(*CreateTDCentricCheck());

	auto seedWidget = CreateSeedWidget();
	auto ridgeSizeWidget = CreateRidgeSizeWidget();
	auto gapSizeWidget = CreateGapSizeWidget();
	auto gratingVelocityWidget = CreateGratingVelocityWidget();

	seedWidget->set_hexpand(true);
	ridgeSizeWidget->set_hexpand(true);
	gapSizeWidget->set_hexpand(true);
	checks->set_hexpand(true);
	gratingVelocityWidget->set_hexpand(true);

	auto grid = Gtk::manage(new Gtk::Grid());
	grid->attach(*seedWidget,0,0,2,1);
	grid->attach(*ridgeSizeWidget,0,1,1,1);
	grid->attach(*gapSizeWidget,1,1,1,1);
	grid->attach(*checks,0,2,1,1); 
	grid->attach(*gratingVelocityWidget,1,2,1,1);
	SetContent(grid);
}

RadialGratingTextureAdvancedWidget::RadialGratingTextureAdvancedWidget(RadialGratingTexturePtr peer) :
	GratingTextureWidgetSet(peer),
	peer_(peer)
{
	Gtk::VBox *vbox = Gtk::manage(new Gtk::VBox);
	vbox->pack_start(seedCtrl_);
	vbox->pack_start(*Gtk::manage(new GratingPitchWidget(peer->GetGrating())));
	vbox->pack_start(*Gtk::manage(new GratingVelocityWidget(peer->GetGrating())));
	vbox->pack_start(*Gtk::manage(new GratingAdvancedButton(peer->GetGrating())),Gtk::PACK_SHRINK);
	vbox->pack_start(tdCentricCtrl_);
	vbox->pack_start(vibCtrl_);
	vbox->pack_start(*Gtk::manage(new TextureMotionCtrl(peer)));

	Gtk::VBox *lbox = Gtk::manage(new Gtk::VBox);
	lbox->pack_start(invertCtrl_, Gtk::PACK_SHRINK);
	lbox->pack_start(ampCtrl_);

	Gtk::HBox *hbox = Gtk::manage(new Gtk::HBox);
	add(*hbox);
	hbox->pack_start(*lbox, Gtk::PACK_SHRINK);
	hbox->pack_start(*vbox);
	hbox->pack_start(preview_, Gtk::PACK_SHRINK);

	show_all_children();
}

} // namespace graphics
} // namespace latero
