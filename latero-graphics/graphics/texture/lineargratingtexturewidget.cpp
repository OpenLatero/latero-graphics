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
		adj_(Gtk::Adjustment::create(peer->GetAngle(),0,360)), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("orientation", adj_, 1, units::degree)));
		adj_->signal_value_changed().connect(
			sigc::mem_fun(*this, &AngleWidget::OnChanged));
	}
	virtual ~AngleWidget() {};
protected:
	void OnChanged()  { peer_->SetAngle(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	LinearGratingTexturePtr peer_;
};

} // namespace


// LinearGratingTextureWidget /////////////////////////////////////////////////////////


void LinearGratingTextureWidget::Create()
{
	using namespace LinearGratingTextureCtrls;

	auto grid = manage(new Gtk::Grid());
	grid->set_hexpand(true);
	grid->set_vexpand(true);
	grid->set_valign(Gtk::ALIGN_CENTER);
	grid->set_column_homogeneous(true);
		
	auto ridgeSizeWidget = CreateRidgeSizeWidget();
	ridgeSizeWidget->set_vexpand(true);
	auto gapSizeWidget = CreateGapSizeWidget();
	gapSizeWidget->set_vexpand(true);
	auto angleWidget = manage(new AngleWidget(peer_));
	angleWidget->set_vexpand(true);
	auto gratingVelocityWidget = CreateGratingVelocityWidget();
	gratingVelocityWidget->set_vexpand(true);
	auto tdCentricCheck = CreateTDCentricCheck();
	tdCentricCheck->set_vexpand(true);
	auto vibCheck = CreateVibCheck();
	vibCheck->set_vexpand(true);

	grid->attach(*ridgeSizeWidget,0,0,1,1); 
	grid->attach(*gapSizeWidget,1,0,1,1);
	grid->attach(*angleWidget,0,1,1,1);
	grid->attach(*gratingVelocityWidget,1,1,1,1);
	grid->attach(*tdCentricCheck,0,2,1,1);
	grid->attach(*vibCheck,1,2,1,1);

	SetContent(grid);
}


LinearGratingTextureAdvancedWidget::LinearGratingTextureAdvancedWidget(LinearGratingTexturePtr peer) :
	GratingTextureWidgetSet(peer)
{
	using namespace LinearGratingTextureCtrls;

	auto gratingPitchWidget = manage(new GratingPitchWidget(peer->GetGrating()));
	auto angleWidget = manage(new AngleWidget(peer));

	seedCtrl_.set_hexpand(true);
	gratingPitchWidget->set_hexpand(true);
	angleWidget->set_hexpand(true);
	gratingCtrls_.velocityWidget_.set_hexpand(true);
	gratingCtrls_.compressCtrl_.set_hexpand(true);
	tdCentricCtrl_.set_hexpand(true);
	vibCtrl_.set_hexpand(true);
	gratingCtrls_.advButton_.set_hexpand(true);	

	seedCtrl_.set_vexpand(true);
	gratingPitchWidget->set_vexpand(true);
	angleWidget->set_vexpand(true);
	gratingCtrls_.velocityWidget_.set_vexpand(true);
	gratingCtrls_.compressCtrl_.set_vexpand(true);
	tdCentricCtrl_.set_vexpand(true);
	vibCtrl_.set_vexpand(true);
	gratingCtrls_.advButton_.set_vexpand(true);	

	auto grid = manage(new Gtk::Grid());
	grid->attach(seedCtrl_,0,2,0,1);
	grid->attach(*gratingPitchWidget,0,1,2,1);
	grid->attach(*angleWidget,0,2,1,1);
	grid->attach(gratingCtrls_.velocityWidget_,1,2,1,1);
	grid->attach(gratingCtrls_.compressCtrl_,0,3,2,1);
	grid->attach(tdCentricCtrl_,0,4,2,1);
	grid->attach(vibCtrl_,0,5,2,1);
	grid->attach(gratingCtrls_.advButton_,0,6,2,1);

	auto lbox = manage(new Gtk::VBox);
	lbox->pack_start(invertCtrl_, Gtk::PACK_SHRINK);
	lbox->pack_start(ampCtrl_);

	pack_start(*lbox, Gtk::PACK_SHRINK);
	pack_start(*grid);
	pack_start(preview_, Gtk::PACK_SHRINK);

	show_all_children();
}

} // namespace graphics
} // namespace latero
