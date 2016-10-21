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

#include "lineargratingtexturewidget.h"
#include "lineargratingtexture.h"
#include <gtkmm/spinbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/table.h>
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "gratingtexturewidget.h"
#include "../../pointwidget.h"

namespace latero {
namespace graphics { 

namespace LinearGratingTextureCtrls {

class AngleWidget : public Gtk::HBox
{
public:
	AngleWidget(LinearGratingTexturePtr peer) :
		adj_(peer->GetAngle(),0,360), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("orientation", adj_, 1, units::degree)));
		adj_.signal_value_changed().connect(
			sigc::mem_fun(*this, &AngleWidget::OnChanged));
	}
	virtual ~AngleWidget() {};
protected:
	void OnChanged()  { peer_->SetAngle(adj_.get_value()); };
	Gtk::Adjustment adj_;
	LinearGratingTexturePtr peer_;
};

} // namespace


// LinearGratingTextureWidget /////////////////////////////////////////////////////////


void LinearGratingTextureWidget::Create()
{
	using namespace LinearGratingTextureCtrls;
	Gtk::Table *table = manage(new Gtk::Table(2,3));
	table->attach(*CreateRidgeSizeWidget(),0,1,0,1); 
	table->attach(*CreateGapSizeWidget(),1,2,0,1);
	table->attach(*manage(new AngleWidget(peer_)),0,1,1,2);
	table->attach(*CreateGratingVelocityWidget(),1,2,1,2);
	table->attach(*CreateTDCentricCheck(),0,1,2,3);
	table->attach(*CreateVibCheck(),1,2,2,3);
	SetContent(table);
}


LinearGratingTextureAdvancedWidget::LinearGratingTextureAdvancedWidget(LinearGratingTexturePtr peer) :
	GratingTextureWidgetSet(peer)
{
	using namespace LinearGratingTextureCtrls;

	Gtk::Table *table = manage(new Gtk::Table(2,7));
	table->attach(seedCtrl_,0,2,0,1);
	table->attach(*manage(new GratingPitchWidget(peer->GetGrating())),0,2,1,2); 
	table->attach(*manage(new AngleWidget(peer)),0,1,2,3);
		table->attach(gratingCtrls_.velocityWidget_,1,2,2,3);
	table->attach(gratingCtrls_.compressCtrl_,0,2,3,4);
	table->attach(tdCentricCtrl_,0,2,4,5);
	table->attach(vibCtrl_,0,2,5,6);
	table->attach(gratingCtrls_.advButton_,0,2,6,7);

	Gtk::VBox *lbox = manage(new Gtk::VBox);
	lbox->pack_start(invertCtrl_, Gtk::PACK_SHRINK);
	lbox->pack_start(ampCtrl_);

	pack_start(*lbox, Gtk::PACK_SHRINK);
	pack_start(*table);
	pack_start(preview_, Gtk::PACK_SHRINK);

	show_all_children();
}

} // namespace graphics
} // namespace latero
