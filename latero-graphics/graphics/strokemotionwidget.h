#pragma once

#include "stroke.h"
#include <gtkmm.h>

namespace latero::graphics {

class StrokeMotionWidget : public Gtk::Box
{
public:
	StrokeMotionWidget(StrokePtr peer);
	virtual ~StrokeMotionWidget() {};
protected:
	void Rebuild();
	void OnCueChanged();

	Gtk::Widget *velWidget_;
	Gtk::Frame holder_;
	StrokePtr peer_;

};

} // namespace

