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

#include "blendtexturewidget.h"
#include "blendtexture.h"
#include "../patternpreview.h"
#include "../modulator/modulator.h"

namespace latero {
namespace graphics { 

BlendTextureWidget::BlendTextureWidget(BlendTexturePtr peer) :
	peer_(peer),
	modCombo_(peer->GetModulator())
{
	modWidgetHolder_.set_shadow_type(Gtk::SHADOW_NONE);

	txWidgets_[0] = manage(new TextureSelectorWidget(peer->GetFirstTexture()));
	txWidgets_[1] = manage(new TextureSelectorWidget(peer->GetSecondTexture()));
	ModulatorPtr mod = peer->GetModulator();

	Gtk::HBox *optionBox = manage(new Gtk::HBox);
	optionBox->pack_start(modCombo_, Gtk::PACK_SHRINK);
	optionBox->pack_start(modWidgetHolder_);
	optionBox->pack_start(*manage(new PatternPreview(peer)),Gtk::PACK_SHRINK);
	modWidgetHolder_.add(*manage(mod->CreateWidget(mod)));

	append_page(*optionBox,"options");
	append_page(*txWidgets_[0],"Texture (high)");
	append_page(*txWidgets_[1],"Texture (low)");

	txWidgets_[0]->SignalTextureChanged().connect(
		sigc::mem_fun(*this, &BlendTextureWidget::OnFirstTextureChanged));
	txWidgets_[1]->SignalTextureChanged().connect(
		sigc::mem_fun(*this, &BlendTextureWidget::OnSecondTextureChanged));
	modCombo_.SignalModulatorChanged().connect(
		sigc::mem_fun(*this, &BlendTextureWidget::OnModulatorChanged));

	show_all_children();
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

	Widget *old = modWidgetHolder_.get_child();
	modWidgetHolder_.remove();
	delete old;

	modWidgetHolder_.add(*manage(mod->CreateWidget(mod)));
	show_all_children();
}

} // namespace graphics
} // namespace latero
