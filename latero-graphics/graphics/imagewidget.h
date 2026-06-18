#pragma once

#include <gtkmm.h>
#include "texture/textureselectorwidget.h"
#include "../maskwidget.h"
#include "image.h"

namespace latero::graphics {

class ImageWidget : public Gtk::Box
{
public:
	ImageWidget(ImagePtr peer, bool showMaskWidget=true);
	virtual ~ImageWidget();

protected:
	void OnTextureChanged();

	TextureSelectorWidget txWidget_;
	MaskWidget maskWidget_;
    ImagePtr peer_;
};

} // namespace

