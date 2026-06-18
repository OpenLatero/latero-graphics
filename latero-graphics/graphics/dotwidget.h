#pragma once

#include <gtkmm.h>
#include "dot.h"


namespace latero::graphics {

class DotRadiusCtrl : public Gtk::Box
{
public:
	DotRadiusCtrl(DotPtr peer);
	virtual ~DotRadiusCtrl() {}
protected:
	void OnChanged();
	DotPtr peer_;
    Glib::RefPtr<Gtk::Adjustment> adj_;
};


class DotHeightCtrl : public Gtk::Box
{
public:
	DotHeightCtrl(DotPtr peer);
	virtual ~DotHeightCtrl() {}
protected:
	void OnChanged();
	DotPtr peer_;
    Glib::RefPtr<Gtk::Adjustment> adj_;
};


class DotWidget : public Gtk::Box
{
public:
	DotWidget(DotPtr peer);
	virtual ~DotWidget() {};
};

} // namespace

