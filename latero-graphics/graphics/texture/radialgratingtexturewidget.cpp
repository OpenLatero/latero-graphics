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

#include <gtkmm.h>
#include "radialgratingtexturewidget.h"
#include "radialgratingtexture.h"

namespace latero {
namespace graphics { 

void RadialGratingTextureWidget::Create()
{
	auto vibCheck = CreateVibCheck();
	auto tdCentricCheck = CreateTDCentricCheck();
	auto seedWidget = CreateSeedWidget();
	auto ridgeSizeWidget = CreateRidgeSizeWidget();
	auto gapSizeWidget = CreateGapSizeWidget();
	auto gratingVelocityWidget = CreateGratingVelocityWidget();

	vibCheck->set_hexpand();
	tdCentricCheck->set_hexpand();
	seedWidget->set_hexpand();
	ridgeSizeWidget->set_hexpand();
	gapSizeWidget->set_hexpand();
	checks->set_hexpand();
	gratingVelocityWidget->set_hexpand();

	auto checks = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	checks->append(*vibCheck);
	checks->append(*tdCentricCheck);

	auto grid = Gtk::manage(new Gtk::Grid());
	grid->attach(*seedWidget,0,0,2,1);
	grid->attach(*ridgeSizeWidget,0,1,1,1);
	grid->attach(*gapSizeWidget,1,1,1,1);
	grid->attach(*checks,0,2,1,1); 
	grid->attach(*gratingVelocityWidget,1,2,1,1);
	SetContent(grid);
}

RadialGratingTextureAdvancedWidget::RadialGratingTextureAdvancedWidget(RadialGratingTexturePtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL),
	GratingTextureWidgetSet(peer),
	peer_(peer)
{
	auto gratingPitchWidget = Gtk::manage(new GratingPitchWidget(peer->GetGrating()));
	auto gratingVelocityWidget = Gtk::manage(new GratingVelocityWidget(peer->GetGrating()));
	auto gratingAdvancedButton = Gtk::manage(new GratingAdvancedButton(peer->GetGrating()));
	auto textureMotionCtrl = Gtk::manage(new TextureMotionCtrl(peer));

	gratingPitchWidget->set_vexpand();
	gratingVelocityWidget->set_vexpand();
	tdCentricCtrl_->set_vexpand();
	vibCtrl_->set_vexpand();
	textureMotionCtrl->set_vexpand();
	ampCtrl_->set_vexpand();
	seedCtrl_->set_vexpand();
	vbox->set_hexpand();

	auto vbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
	vbox->append(seedCtrl_);
	vbox->append(*gratingPitchWidget);
	vbox->append(*gratingVelocityWidget);
	vbox->append(*gratingAdvancedButton);
	vbox->append(tdCentricCtrl_);
	vbox->append(vibCtrl_);
	vbox->append(*textureMotionCtrl);

	auto lbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
	lbox->append(invertCtrl_);
	lbox->append(ampCtrl_);

	auto hbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	add(*hbox);
	hbox->append(*lbox);
	hbox->append(*vbox);
	hbox->append(preview_);

	show_all_children();
}

} // namespace graphics
} // namespace latero
