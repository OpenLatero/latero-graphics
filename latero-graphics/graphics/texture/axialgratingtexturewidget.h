#pragma once

//TODO: check why this is required
#include "axialgratingtexture.h"
#include "gratingtexturewidget.h"

namespace latero::graphics {

class AxialGratingTextureWidget : public GratingTextureWidget
{
public:
	AxialGratingTextureWidget(AxialGratingTexturePtr peer) : GratingTextureWidget(peer), peer_(peer) { Create(); }
	virtual ~AxialGratingTextureWidget() {};
protected:
	virtual void Rebuild() { Create(); };
	void Create();
	AxialGratingTexturePtr peer_;
};


class AxialGratingTextureAdvancedWidget : public Gtk::Box, public GratingTextureWidgetSet
{
public:
	AxialGratingTextureAdvancedWidget(AxialGratingTexturePtr peer);
	virtual ~AxialGratingTextureAdvancedWidget() {}
protected:
	AxialGratingTexturePtr peer_;
};

} // namespace

