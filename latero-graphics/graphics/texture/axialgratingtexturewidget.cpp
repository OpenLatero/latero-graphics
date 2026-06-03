#include "axialgratingtexturewidget.h"
#include <gtkmm.h>
#include "axialgratingtexture.h"
#include "../../gtk/numwidget.h"

namespace latero::graphics {
namespace AxialGratingTextureCtrls {

class SeedAngleCtrl : public Gtk::Box
{
public:
	SeedAngleCtrl(AxialGratingTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetSeedAngle(),0,360)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 0, "seed angle", units::degree));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &SeedAngleCtrl::OnChanged));
	}
	virtual ~SeedAngleCtrl() {};
protected:
	void OnChanged() { peer_->SetSeedAngle(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	AxialGratingTexturePtr peer_;
};

} // namespace AxialGratingTextureCtrls


void AxialGratingTextureWidget::Create()
{
	using namespace AxialGratingTextureCtrls;

	auto seedWidget = CreateSeedWidget();
	auto seedAngleCtrl = Gtk::make_managed<SeedAngleCtrl>(peer_);
	auto gratingPitchWidget = Gtk::make_managed<GratingPitchWidget>(peer_->GetGrating());
	auto vibCheck = CreateVibCheck();
	auto tdCenticCheck = CreateTDCentricCheck();
	auto gratingVelocityWidget = CreateGratingVelocityWidget();

	seedWidget->set_hexpand();
	seedAngleCtrl->set_hexpand();
	gratingPitchWidget->set_hexpand();
	vibCheck->set_hexpand();
	tdCenticCheck->set_hexpand();
	gratingVelocityWidget->set_hexpand();

	auto grid = Gtk::make_managed<Gtk::Grid>();
	grid->attach(*seedWidget,0,0,2,1);
	grid->attach(*seedAngleCtrl,2,0,1,1);
	grid->attach(*gratingPitchWidget,0,1,3,1);
	grid->attach(*vibCheck,0,2,1,1);
	grid->attach(*tdCenticCheck,1,2,1,1);
	grid->attach(*gratingVelocityWidget,2,2,1,1);
	SetContent(grid);
}

AxialGratingTextureAdvancedWidget::AxialGratingTextureAdvancedWidget(AxialGratingTexturePtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL),
	GratingTextureWidgetSet(peer),
	peer_(peer)
{
	using namespace AxialGratingTextureCtrls;

	auto rbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto seedBox  = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	auto gratingPitchWidget = Gtk::make_managed<GratingPitchWidget>(peer->GetGrating());
	auto gratingVelocityWidget = Gtk::make_managed<GratingVelocityWidget>(peer->GetGrating());
	auto gratingAdvancedButton = Gtk::make_managed<GratingAdvancedButton>(peer->GetGrating());
	auto seedAngleCtrl = Gtk::make_managed<SeedAngleCtrl>(peer);

	seedBox->set_vexpand();
	gratingPitchWidget->set_vexpand();
	gratingVelocityWidget->set_vexpand();
	tdCentricCtrl_.set_vexpand();
	vibCtrl_.set_vexpand();
	seedCtrl_.set_hexpand();
	seedAngleCtrl->set_hexpand();

	seedBox->append(seedCtrl_);
	seedBox->append(*seedAngleCtrl);
	rbox->append(*seedBox);
	rbox->append(*gratingPitchWidget);
	rbox->append(*gratingVelocityWidget);
	rbox->append(*gratingAdvancedButton);
	rbox->append(tdCentricCtrl_);
	rbox->append(vibCtrl_);

	auto lbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	lbox->append(invertCtrl_);
	lbox->append(ampCtrl_);
	ampCtrl_.set_vexpand();

	auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	append(*hbox);
	hbox->append(*lbox);
	hbox->append(*rbox);
	rbox->set_hexpand();
	hbox->append(preview_);
}

} // namespace
