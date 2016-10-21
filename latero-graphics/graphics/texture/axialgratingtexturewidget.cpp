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

#include "axialgratingtexturewidget.h"
#include <gtkmm/table.h>
#include "axialgratingtexture.h"
#include "../../gtk/numwidget.h"

namespace latero {
namespace graphics { 

namespace AxialGratingTextureCtrls {

class SeedAngleCtrl : public Gtk::HBox
{
public:
	SeedAngleCtrl(AxialGratingTexturePtr peer) :
		adj_(peer->GetSeedAngle(),0,360), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("seed angle", adj_, 0, units::degree)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &SeedAngleCtrl::OnChanged));
	}
	virtual ~SeedAngleCtrl() {};
protected:
	void OnChanged() { peer_->SetSeedAngle(adj_.get_value()); }
	Gtk::Adjustment adj_;
	AxialGratingTexturePtr peer_;
};

} // namespace AxialGratingTextureCtrls


void AxialGratingTextureWidget::Create()
{
	using namespace AxialGratingTextureCtrls;
	Gtk::Table *table = manage(new Gtk::Table(3,3));
	table->attach(*CreateSeedWidget(),0,2,0,1);
	table->attach(*manage(new SeedAngleCtrl(peer_)),2,3,0,1);
	table->attach(*manage(new GratingPitchWidget(peer_->GetGrating())),0,3,1,2);
	table->attach(*CreateVibCheck(),0,1,2,3);
	table->attach(*CreateTDCentricCheck(),1,2,2,3);
	table->attach(*CreateGratingVelocityWidget(),2,3,2,3);
	SetContent(table);
}

AxialGratingTextureAdvancedWidget::AxialGratingTextureAdvancedWidget(AxialGratingTexturePtr peer) :
	GratingTextureWidgetSet(peer),
	peer_(peer)
{
	using namespace AxialGratingTextureCtrls;

	Gtk::VBox *rbox = manage(new Gtk::VBox);
	Gtk::HBox *seedBox  = manage(new Gtk::HBox);

	seedBox->pack_start(seedCtrl_);
	seedBox->pack_start(*manage(new SeedAngleCtrl(peer)));
	rbox->pack_start(*seedBox);
	rbox->pack_start(*manage(new GratingPitchWidget(peer->GetGrating())));
	rbox->pack_start(*manage(new GratingVelocityWidget(peer->GetGrating())));
	rbox->pack_start(*manage(new GratingAdvancedButton(peer->GetGrating())),Gtk::PACK_SHRINK);
	rbox->pack_start(tdCentricCtrl_);
	rbox->pack_start(vibCtrl_);

	Gtk::VBox *lbox = manage(new Gtk::VBox);
	lbox->pack_start(invertCtrl_, Gtk::PACK_SHRINK);
	lbox->pack_start(ampCtrl_);

	Gtk::HBox *hbox = manage(new Gtk::HBox);
	add(*hbox);
	hbox->pack_start(*lbox, Gtk::PACK_SHRINK);
	hbox->pack_start(*rbox);
	hbox->pack_start(preview_, Gtk::PACK_SHRINK);

	show_all_children();
}

} // namespace graphics
} // namespace latero
