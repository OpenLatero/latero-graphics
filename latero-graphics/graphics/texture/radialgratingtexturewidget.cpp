#include <gtkmm.h>
#include "radialgratingtexturewidget.h"
#include "radialgratingtexture.h"

namespace latero::graphics {

void RadialGratingTextureWidget::Create()
{
	auto vibCheck = CreateVibCheck();
	auto tdCentricCheck = CreateTDCentricCheck();
	auto seedWidget = CreateSeedWidget();
	auto ridgeSizeWidget = CreateRidgeSizeWidget();
	auto gapSizeWidget = CreateGapSizeWidget();
	auto gratingVelocityWidget = CreateGratingVelocityWidget();
	auto checks = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	auto grid = Gtk::make_managed<Gtk::Grid>();

	vibCheck->set_hexpand();
	tdCentricCheck->set_hexpand();
	seedWidget->set_hexpand();
	ridgeSizeWidget->set_hexpand();
	gapSizeWidget->set_hexpand();
	checks->set_hexpand();
	gratingVelocityWidget->set_hexpand();

	checks->append(*vibCheck);
	checks->append(*tdCentricCheck);

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
	auto gratingPitchWidget = Gtk::make_managed<GratingPitchWidget>(peer->GetGrating());
	auto gratingVelocityWidget = Gtk::make_managed<GratingVelocityWidget>(peer->GetGrating());
	auto gratingAdvancedButton = Gtk::make_managed<GratingAdvancedButton>(peer->GetGrating());
	auto textureMotionCtrl = Gtk::make_managed<TextureMotionCtrl>(peer);
	auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);

	gratingPitchWidget->set_vexpand();
	gratingVelocityWidget->set_vexpand();
	tdCentricCtrl_.set_vexpand();
	vibCtrl_.set_vexpand();
	textureMotionCtrl->set_vexpand();
	ampCtrl_.set_vexpand();
	seedCtrl_.set_vexpand();
	vbox->set_hexpand();

	vbox->append(seedCtrl_);
	vbox->append(*gratingPitchWidget);
	vbox->append(*gratingVelocityWidget);
	vbox->append(*gratingAdvancedButton);
	vbox->append(tdCentricCtrl_);
	vbox->append(vibCtrl_);
	vbox->append(*textureMotionCtrl);

	auto lbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	lbox->append(invertCtrl_);
	lbox->append(ampCtrl_);

	auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	append(*hbox);
	hbox->append(*lbox);
	hbox->append(*vbox);
	hbox->append(preview_);
}

} // namespace
