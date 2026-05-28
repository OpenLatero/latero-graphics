#pragma once

#include <gtkmm.h>
#include "plaintexturefwd.h"

namespace latero::graphics {

class PlainTextureWidget : public Gtk::Box
{
public:
	PlainTextureWidget(PlainTexturePtr peer);
	virtual ~PlainTextureWidget() {}

protected:
	PlainTexturePtr peer_;
};

} // namespace

