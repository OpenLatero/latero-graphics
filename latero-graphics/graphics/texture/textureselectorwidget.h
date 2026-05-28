#pragma once

#include "texturedropdown.h"
#include <gtkmm.h>
#include "../../pointwidget.h"

namespace latero::graphics {

class TextureSelectorCtrl : public Gtk::Box
{
public:
	TextureSelectorCtrl(TexturePtr texture);
	virtual ~TextureSelectorCtrl() {}

	void SetTexture(TexturePtr tx) { texture_ = tx; SignalTextureChanged_(); }
	TexturePtr GetTexture() { return texture_; }

	void OnModeChanged(TexturePtr tx);

	sigc::signal<void()> SignalTextureChanged() { return SignalTextureChanged_; }

protected:
	void OnSave();
	void OnLoad();

	sigc::signal<void()> SignalTextureChanged_;
	TextureDropDown modeDropDown_;
    TexturePtr texture_;
};

class TextureSelectorWidget : public Gtk::Box
{
public:
	TextureSelectorWidget(TexturePtr texture);
	virtual ~TextureSelectorWidget();
	TexturePtr GetTexture() { return ctrl_.GetTexture(); }
	void SetTexture(TexturePtr tx) { ctrl_.SetTexture(tx); }
	sigc::signal<void()> SignalTextureChanged() { return SignalTextureChanged_; }

protected:
	sigc::signal<void()> SignalTextureChanged_;

	void OnChanged();
	void Build();
	void Rebuild();

	TextureSelectorCtrl ctrl_;
	Widget *widget_;
};

} // namespace

