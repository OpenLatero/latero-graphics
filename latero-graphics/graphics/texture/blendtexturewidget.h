#pragma once

#include "blendtexture.h"
#include "textureselectorwidget.h"
#include <gtkmm.h>
#include "../modulator/modulatordropdown.h"

namespace latero::graphics {

class BlendTextureWidget : public Gtk::Notebook
{
public:
	BlendTextureWidget(BlendTexturePtr peer);
	virtual ~BlendTextureWidget() {}
protected:
	void OnFirstTextureChanged();
	void OnSecondTextureChanged();
	void OnModulatorChanged(ModulatorPtr mod);

	BlendTexturePtr peer_;
	Gtk::Frame modWidgetHolder_;
	ModulatorDropDown modDropDown_;
	TextureSelectorWidget *txWidgets_[2];
};

} // namespace

