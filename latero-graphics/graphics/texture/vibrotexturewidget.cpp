#include "vibrotexturewidget.h"
#include "vibrotexture.h"
#include "texturewidget.h"
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "../../graphics/patternpreview.h"

namespace latero::graphics {

class VibroTextureFreqCtrl : public Gtk::Box
{
public:
	VibroTextureFreqCtrl(VibroTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetFreq(),Oscillator::freq_min,Oscillator::freq_max)), peer_(peer)
	{
		auto widget = Gtk::make_managed<gtk::HVNumWidget>(Gtk::Orientation::HORIZONTAL, "frequency", adj_,1, units::hz);
		append(*widget);
		widget->set_vexpand();
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &VibroTextureFreqCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetFreq(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	VibroTexturePtr peer_;
};


VibroTextureWidget::VibroTextureWidget(VibroTexturePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	auto sidebox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto textureInvertCtrl = Gtk::make_managed<TextureInvertCtrl>(peer);
	auto textureAmplitudeCtrl = Gtk::make_managed<TextureAmplitudeCtrl>(peer);
	auto vibroTextureFreqCtrl = Gtk::make_managed<VibroTextureFreqCtrl>(peer);
	auto patternPreview = Gtk::make_managed<PatternPreview>(peer);

	textureAmplitudeCtrl->set_vexpand();
	vibroTextureFreqCtrl->set_hexpand();

	sidebox->append(*textureInvertCtrl);
	sidebox->append(*textureAmplitudeCtrl);
	append(*sidebox);
	append(*vibroTextureFreqCtrl);
	append(*patternPreview);
}

} // namespace
