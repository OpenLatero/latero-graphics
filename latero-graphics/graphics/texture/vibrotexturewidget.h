#pragma once

#include <gtkmm.h>
#include "vibrotexturefwd.h"

namespace latero::graphics {

class VibroTextureWidget : public Gtk::Box
{
public:
	VibroTextureWidget(VibroTexturePtr peer);
	virtual ~VibroTextureWidget() {}

protected:
	VibroTexturePtr peer_;
};

} // namespace

