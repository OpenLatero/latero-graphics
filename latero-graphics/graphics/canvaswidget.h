#pragma once

#include "canvas.h"
#include <gtkmm.h>

namespace latero::graphics {

class CanvasWidget : public Gtk::Box
{
public:
	CanvasWidget(CanvasPtr peer);
	virtual ~CanvasWidget() {};
};

} // namespace

