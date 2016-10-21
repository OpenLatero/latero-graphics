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

#include "gratingtexturewidget.h"
#include "gratingtexture.h"
#include "../../gtk/numwidget.h"
#include <gtkmm/notebook.h>

namespace latero {
namespace graphics { 


// GratingTextureSeedCtrl ///////////////////////////////////////////////////////////////

GratingTextureSeedCtrl::GratingTextureSeedCtrl(GratingTexturePtr peer) :
	Gtk::Frame("seed"),
	peer_(peer),
	widget_(peer->GetSeed(),0,peer->Dev()->GetSurfaceWidth(),0,peer->Dev()->GetSurfaceHeight())
{
	add(widget_);
	widget_.SignalValueChanged().connect(
		sigc::mem_fun(*this, &GratingTextureSeedCtrl::OnChanged));

}
void GratingTextureSeedCtrl::OnChanged() { peer_->SetSeed(widget_.GetValue()); }


GratingTextureWidgetSet::GratingTextureWidgetSet(GratingTexturePtr peer) :
	preview_(peer),
	vibCtrl_(peer->GetOscillator(),true),
	invertCtrl_(peer),
	tdCentricCtrl_(peer),
	ampCtrl_(peer),
	seedCtrl_(peer),
    gratingCtrls_(peer->GetGrating())
{
}

GratingRidgeSizeWidget* GratingTextureWidget::CreateRidgeSizeWidget()
{ return manage(new GratingRidgeSizeWidget(peer_->GetGrating())); }

GratingGapSizeWidget* GratingTextureWidget::CreateGapSizeWidget()
{ return manage(new GratingGapSizeWidget(peer_->GetGrating())); }

GratingVelocityWidget* GratingTextureWidget::CreateGratingVelocityWidget()
{ return manage(new GratingVelocityWidget(peer_->GetGrating())); }

GratingTextureSeedCtrl* GratingTextureWidget::CreateSeedWidget()
{ return manage(new GratingTextureSeedCtrl(peer_)); }

GratingRegularizeButton *GratingTextureWidget::CreateRegularizeButton()
{ return manage(new GratingRegularizeButton(peer_->GetGrating())); }


} // namespace graphics
} // namespace latero
