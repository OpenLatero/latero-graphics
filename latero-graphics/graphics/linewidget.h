#pragma once

#include "line.h"
#include <gtkmm.h>

namespace latero::graphics {

class LineWidget : public Gtk::Notebook
{
public:
	LineWidget(LinePtr peer);
	virtual ~LineWidget() {};
};

} // namespace

