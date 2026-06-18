#pragma once

#include "toggle.h"
#include <gtkmm.h>

namespace latero::graphics {

class ToggleWidget : public Gtk::Notebook
{
public:
	ToggleWidget(TogglePtr peer);
	virtual ~ToggleWidget() {};
};

} // namespace

