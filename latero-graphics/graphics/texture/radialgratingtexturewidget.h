#pragma once

//TODO: check why this is required
#include "radialgratingtexture.h"
#include "gratingtexturewidget.h"

namespace latero::graphics {

class RadialGratingTextureWidget : public GratingTextureWidget
{
public:
	RadialGratingTextureWidget(RadialGratingTexturePtr peer) : GratingTextureWidget(peer), peer_(peer) { Create(); }
	virtual ~RadialGratingTextureWidget() {};
protected:
	virtual void Rebuild() { Create(); };
	void Create();
	RadialGratingTexturePtr peer_;
};

class RadialGratingTextureAdvancedWidget : public Gtk::Box, public GratingTextureWidgetSet
{
public:
	RadialGratingTextureAdvancedWidget(RadialGratingTexturePtr peer);
	virtual ~RadialGratingTextureAdvancedWidget() {}
protected:
	RadialGratingTexturePtr peer_;
};

} // namespace

