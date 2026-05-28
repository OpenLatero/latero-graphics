#include "plaintexturewidget.h"
#include "plaintexture.h"
#include "texturewidget.h"
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "../patternpreview.h"
#include "../../oscillatorwidget.h"

namespace latero::graphics {

PlainTextureWidget::PlainTextureWidget(PlainTexturePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	auto sidebox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto textureInvertCtrl = Gtk::make_managed<TextureInvertCtrl>(peer);
	auto textureAmplitudeCtrl = Gtk::make_managed<TextureAmplitudeCtrl>(peer);
	auto oscillatorWidget = Gtk::make_managed<OscillatorWidget>(peer->GetOscillator());
	auto patternPreview = Gtk::make_managed<PatternPreview>(peer);

	textureAmplitudeCtrl->set_vexpand();
	oscillatorWidget->set_hexpand();

	sidebox->append(*textureInvertCtrl);
	sidebox->append(*textureAmplitudeCtrl);
	append(*sidebox);
	append(*oscillatorWidget);
	append(*patternPreview);
}

} // namespace
