#pragma once

#include "linefwd.h"
#include <gtkmm.h>

namespace latero::graphics {

class LineWidget : public Gtk::Notebook
{
public:
	LineWidget(LinePtr peer);
	virtual ~LineWidget() {};
};

} // namespace

