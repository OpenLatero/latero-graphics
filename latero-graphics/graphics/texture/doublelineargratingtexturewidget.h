#pragma once

#include <gtkmm.h>
//TODO: check why this is required
#include "doublelineargratingtexture.h"
#include "texturewidget.h"

namespace latero::graphics {

class DoubleLinearGratingTextureWidget : public TextureWidget
{
public:
	DoubleLinearGratingTextureWidget(DoubleLinearGratingTexturePtr peer) : TextureWidget(peer), peer_(peer) { Create(); }
	virtual ~DoubleLinearGratingTextureWidget() {};
protected:
	virtual void Rebuild() { Create(); };
	void Create();
	DoubleLinearGratingTexturePtr peer_;
};


class DoubleLinearGratingTextureAdvancedWidget : public Gtk::Grid
{
public:
	DoubleLinearGratingTextureAdvancedWidget(DoubleLinearGratingTexturePtr peer);
	virtual ~DoubleLinearGratingTextureAdvancedWidget() {}
protected:
	DoubleLinearGratingTexturePtr peer_;
};

} // namespace

