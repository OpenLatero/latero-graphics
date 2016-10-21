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

#ifndef LATERO_GRAPHICS_PLANAR_TEXTURE_SELECTOR_WIDGET
#define LATERO_GRAPHICS_PLANAR_TEXTURE_SELECTOR_WIDGET

#include "texturecombo.h"
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/expander.h>
#include "../../pointwidget.h"

namespace latero {
namespace graphics { 

class TextureSelectorCtrl : public Gtk::VBox
{
public:
	TextureSelectorCtrl(TexturePtr texture);
	virtual ~TextureSelectorCtrl() {}

	void SetTexture(TexturePtr tx) { texture_ = tx; SignalTextureChanged_(); }
	TexturePtr GetTexture() { return texture_; }

	void OnModeChanged(TexturePtr tx);

	sigc::signal<void> SignalTextureChanged() { return SignalTextureChanged_; }

protected:
	void OnSave();
	void OnLoad();

	sigc::signal<void> SignalTextureChanged_;
	TextureCombo modeCombo_;
    TexturePtr texture_;
};

class TextureSelectorWidget : public Gtk::HBox
{
public:
	TextureSelectorWidget(TexturePtr texture);
	virtual ~TextureSelectorWidget();
	TexturePtr GetTexture() { return ctrl_.GetTexture(); }
	void SetTexture(TexturePtr tx) { ctrl_.SetTexture(tx); }
	sigc::signal<void> SignalTextureChanged() { return SignalTextureChanged_; }

protected:
	sigc::signal<void> SignalTextureChanged_;

	void OnChanged();
	void Build();
	void Rebuild();

	TextureSelectorCtrl ctrl_;
	Widget *widget_;
};

} // namespace graphics
} // namespace latero

#endif
