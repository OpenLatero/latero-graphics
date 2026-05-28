#pragma once

#include "polygonfwd.h"
#include <gtkmm.h>

namespace latero::graphics {

class PolygonWidget : public Gtk::Notebook
{
public:
	PolygonWidget(PolygonPtr peer);
	virtual ~PolygonWidget() {};
};

} // namespace

