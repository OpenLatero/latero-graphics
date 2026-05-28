#pragma once

#include <gtkmm.h>
//TODO: check why this is required
#include "lineargratingtexture.h"
#include "gratingtexturewidget.h"

namespace latero::graphics {

class LinearGratingTextureWidget : public GratingTextureWidget
{
public:
	LinearGratingTextureWidget(LinearGratingTexturePtr peer) : GratingTextureWidget(peer), peer_(peer) { Create(); }
	virtual ~LinearGratingTextureWidget() {};
protected:
	virtual void Rebuild() { Create(); };
	void Create();
	LinearGratingTexturePtr peer_;
};

class LinearGratingTextureAdvancedWidget : public Gtk::Box, public GratingTextureWidgetSet
{
public:
	LinearGratingTextureAdvancedWidget(LinearGratingTexturePtr peer);
	virtual ~LinearGratingTextureAdvancedWidget() {}
};

} // namespace

