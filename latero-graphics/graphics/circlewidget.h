#pragma once

#include "circle.h"
#include <gtkmm.h>

namespace latero::graphics {

class CircleWidget : public Gtk::Notebook
{
public:
	CircleWidget(CirclePtr peer);
	virtual ~CircleWidget() {};
};

} // namespace

