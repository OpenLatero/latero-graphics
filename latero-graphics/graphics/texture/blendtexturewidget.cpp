#include "blendtexturewidget.h"
#include "blendtexture.h"
#include "../patternpreview.h"
#include "../modulator/modulator.h"

namespace latero::graphics {

BlendTextureWidget::BlendTextureWidget(BlendTexturePtr peer) :
	peer_(peer),
	modDropDown_(peer->GetModulator())
{
	txWidgets_[0] = Gtk::make_managed<TextureSelectorWidget>(peer->GetFirstTexture());
	txWidgets_[1] = Gtk::make_managed<TextureSelectorWidget>(peer->GetSecondTexture());
	ModulatorPtr mod = peer->GetModulator();

	auto optionBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	optionBox->append(modDropDown_);
	optionBox->append(modWidgetHolder_);
	modWidgetHolder_.set_hexpand();
	optionBox->append(*Gtk::make_managed<PatternPreview>(peer));
	modWidgetHolder_.set_child(*mod->CreateWidget(mod));

	append_page(*optionBox,"options");
	append_page(*txWidgets_[0],"Texture (high)");
	append_page(*txWidgets_[1],"Texture (low)");

	txWidgets_[0]->SignalTextureChanged().connect(
		sigc::mem_fun(*this, &BlendTextureWidget::OnFirstTextureChanged));
	txWidgets_[1]->SignalTextureChanged().connect(
		sigc::mem_fun(*this, &BlendTextureWidget::OnSecondTextureChanged));
	modDropDown_.SignalModulatorChanged().connect(
		sigc::mem_fun(*this, &BlendTextureWidget::OnModulatorChanged));
}

void BlendTextureWidget::OnFirstTextureChanged()
{
	peer_->SetFirstTexture(txWidgets_[0]->GetTexture());
}

void BlendTextureWidget::OnSecondTextureChanged()
{
	peer_->SetSecondTexture(txWidgets_[1]->GetTexture());
}

void BlendTextureWidget::OnModulatorChanged(ModulatorPtr mod)
{
	peer_->SetModulator(mod);

	modWidgetHolder_.unset_child();
	modWidgetHolder_.set_child(*mod->CreateWidget(mod));
}

} // namespace
