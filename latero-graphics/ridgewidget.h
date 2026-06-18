#pragma once

#include <gtkmm.h>
#include "gtk/checkframe.h"
#include "ridge.h"
#include "ridgegraph.h"

namespace latero::graphics {


class RidgeEdgeWidthScale : public Gtk::Box
{
public:
	RidgeEdgeWidthScale(RidgePtr peer);
	virtual ~RidgeEdgeWidthScale() {};
private:
	void OnChange();
	RidgePtr peer_;
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class RidgeTextureCtrl : public gtk::CheckFrame
{
public:
	RidgeTextureCtrl(RidgePtr peer);
	virtual ~RidgeTextureCtrl() {};
protected:
	void OnClick();
	RidgePtr peer_;
};


class RidgeControls : public Gtk::Box
{
public:
	RidgeControls(RidgePtr peer);
protected:
	RidgePtr peer_;
	RidgeEdgeWidthScale edgeWidthScale_;
	RidgeTextureCtrl txCtrl_;
};


class RidgeWidget : public Gtk::Box
{
public:
	RidgeWidget(RidgePtr peer);
protected:
	RidgeControls controls_;
	RidgePtr peer_;
	RidgeGraph *graph_;
};

} // namespace

