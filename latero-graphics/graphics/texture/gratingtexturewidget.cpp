#include "gratingtexturewidget.h"
#include "gratingtexture.h"
#include "../../gtk/numwidget.h"

namespace latero::graphics {


// GratingTextureSeedCtrl ///////////////////////////////////////////////////////////////

GratingTextureSeedCtrl::GratingTextureSeedCtrl(GratingTexturePtr peer) :
	Gtk::Frame("seed"),
	peer_(peer),
	widget_(peer->GetSeed(),0,peer->Dev()->GetSurfaceWidth(),0,peer->Dev()->GetSurfaceHeight())
{
	set_child(widget_);
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
{ return Gtk::make_managed<GratingRidgeSizeWidget>(peer_->GetGrating()); }

GratingGapSizeWidget* GratingTextureWidget::CreateGapSizeWidget()
{ return Gtk::make_managed<GratingGapSizeWidget>(peer_->GetGrating()); }

GratingVelocityWidget* GratingTextureWidget::CreateGratingVelocityWidget()
{ return Gtk::make_managed<GratingVelocityWidget>(peer_->GetGrating()); }

GratingTextureSeedCtrl* GratingTextureWidget::CreateSeedWidget()
{ return Gtk::make_managed<GratingTextureSeedCtrl>(peer_); }

GratingRegularizeButton *GratingTextureWidget::CreateRegularizeButton()
{ return Gtk::make_managed<GratingRegularizeButton>(peer_->GetGrating()); }


} // namespace
