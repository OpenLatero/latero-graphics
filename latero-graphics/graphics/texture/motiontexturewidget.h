#pragma once

#include <gtkmm.h>
#include "motiontexturefwd.h"
#include "textureselectorwidget.h"

namespace latero::graphics {

class MotionTextureWidget : public Gtk::Notebook
{
public:
	MotionTextureWidget(MotionTexturePtr peer);
	virtual ~MotionTextureWidget() {}
protected:
	void OnTextureChanged();

	MotionTexturePtr peer_;
	TextureSelectorWidget txWidget_;
};

} // namespace

