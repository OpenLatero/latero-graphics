#pragma once

#include <gtkmm.h>
#include "dots.h"

namespace latero::graphics {

class DotsRadiusCtrl : public Gtk::Box
{
public:
	DotsRadiusCtrl(DotsPtr peer);
	virtual ~DotsRadiusCtrl() {}
protected:
	void OnChanged();
	DotsPtr peer_;
    Glib::RefPtr<Gtk::Adjustment> adj_;
};


class DotsHeightCtrl : public Gtk::Box
{
public:
	DotsHeightCtrl(DotsPtr peer);
	virtual ~DotsHeightCtrl() {}
protected:
	void OnChanged();
	DotsPtr peer_;
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class DotsWidget : public Gtk::Box
{
public:
	DotsWidget(DotsPtr peer);
	virtual ~DotsWidget() {};
};

} // namespace

