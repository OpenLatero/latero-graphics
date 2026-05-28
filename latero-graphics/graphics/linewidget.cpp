#include "linewidget.h"
#include <iostream>
#include "line.h"
#include "strokewidget.h"
#include "../pointwidget.h"
#include "dotpattern.h"
#include "../gtk/numwidget.h"
#include "patternpreview.h"

namespace latero::graphics {

class LineStartCtrl : public Gtk::Frame
{
public:
	LineStartCtrl(LinePtr peer) :
		Gtk::Frame("start point"),
		peer_(peer),
		widget_(peer->GetStartPoint(),0,peer->Dev()->GetSurfaceWidth(),0,peer->Dev()->GetSurfaceHeight())
	{
		set_child(widget_);
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &LineStartCtrl::OnChanged));
	}
	virtual ~LineStartCtrl() {};
protected:
	void OnChanged() { peer_->SetStartPoint(widget_.GetValue()); }
	LinePtr peer_;
	PointWidget widget_;
};


class LineEndCtrl : public Gtk::Frame
{
public:
	LineEndCtrl(LinePtr peer) :
		Gtk::Frame("end point"),
		peer_(peer),
		widget_(peer->GetEndPoint(),0,peer->Dev()->GetSurfaceWidth(),0,peer->Dev()->GetSurfaceHeight())
	{
		set_child(widget_);
		widget_.SignalValueChanged().connect(sigc::mem_fun(*this, &LineEndCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetEndPoint(widget_.GetValue()); }
	LinePtr peer_;
	PointWidget widget_;
};

class LinePropertiesCtrl : public Gtk::Box
{
public:
	LinePropertiesCtrl(LinePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL)
	{
		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
		box->append(*Gtk::make_managed<LineStartCtrl>(peer));
		box->append(*Gtk::make_managed<LineEndCtrl>(peer));

		append(*box);
		box->set_hexpand();
		append(*Gtk::make_managed<PatternPreview>(peer));
	}
	virtual ~LinePropertiesCtrl() {};
};

LineWidget::LineWidget(LinePtr peer)
{
	append_page(*Gtk::make_managed<LinePropertiesCtrl>(peer), "properties");
	append_page(*Gtk::make_managed<StrokeProfileWidget>(peer->GetStroke()), "stroke");
	append_page(*Gtk::make_managed<StrokeFillWidget>(peer->GetStroke()), "fill");
	append_page(*Gtk::make_managed<StrokeMotionWidget>(peer->GetStroke()),"motion");
	append_page(*Gtk::make_managed<StrokeDottedWidget>(peer->GetStroke()),"dots");
}


} // namespace

