#pragma once

#include <gtkmm.h>
#include "dotpatternfwd.h"

namespace latero::graphics {

class DotPatternMinSpacingCtrl : public Gtk::Box
{
public:
	DotPatternMinSpacingCtrl(DotPatternPtr peer);
	virtual ~DotPatternMinSpacingCtrl() {}
protected:
	void OnChanged();
    Glib::RefPtr<Gtk::Adjustment> adj_;
	DotPatternPtr peer_;
};


class DotPatternWidget : public Gtk::Box
{
public:
	DotPatternWidget(DotPatternPtr peer);
	virtual ~DotPatternWidget() {};
};

} // namespace

