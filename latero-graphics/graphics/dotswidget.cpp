#include "dotswidget.h"
#include "dots.h"
#include "../gtk/numwidget.h"
#include "../ridgewidget.h"
#include <gtkmm.h>
#include "../pointwidget.h"
#include "../oscillatorwidget.h"
#include "../pointlistwidget.h"

namespace latero::graphics {

class DotsPointsWidget : public Gtk::Frame
{
public:
	DotsPointsWidget(DotsPtr peer) : 
		Gtk::Frame("points"), widget_(peer->GetPoints()), peer_(peer)
	{
		set_child(widget_);
		widget_.SignalChanged().connect(sigc::mem_fun(*this, &DotsPointsWidget::OnChanged));
	}
	~DotsPointsWidget() {}
protected:
	void OnChanged() { peer_->SetPoints(widget_.GetPoints()); }
	PointListWidget widget_;
	DotsPtr peer_;
};

DotsRadiusCtrl::DotsRadiusCtrl(DotsPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	peer_(peer),
	adj_(Gtk::Adjustment::create(peer->GetDotRadius(), 0.01, 20.0))
{
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DotsRadiusCtrl::OnChanged));
	append(*Gtk::make_managed<gtk::HNumWidget>("radius",adj_,1,"mm"));
}
void DotsRadiusCtrl::OnChanged() { peer_->SetDotRadius(adj_->get_value()); }


DotsHeightCtrl::DotsHeightCtrl(DotsPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	peer_(peer),
	adj_(Gtk::Adjustment::create(peer->GetHeight()*100, 1, 100))
{
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DotsHeightCtrl::OnChanged));
	append(*Gtk::make_managed<gtk::HNumWidget>("height",adj_,0,"%"));
}
void DotsHeightCtrl::OnChanged() { peer_->SetHeight(adj_->get_value()/100); };


DotsWidget::DotsWidget(DotsPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL)
{
	auto grid = Gtk::make_managed<Gtk::Grid>();
	grid->attach(*Gtk::make_managed<DotsRadiusCtrl>(peer),0,0,2,1);
	grid->attach(*Gtk::make_managed<DotsHeightCtrl>(peer),0,1,1,1);
	grid->attach(*Gtk::make_managed<RidgeEdgeWidthScale>(peer->GetProfile()),1,1,1,1);
	grid->attach(*Gtk::make_managed<RidgeTextureCtrl>(peer->GetProfile()),0,2,2,1);
	grid->attach(*Gtk::make_managed<OscillatorWidget>(peer->GetOscillator()),0,3,2,1);
	grid->attach(*Gtk::make_managed<DotsPointsWidget>(peer),2,0,1,4);

	grid->set_hexpand();

	append(*grid);
	append(*Gtk::make_managed<RidgeGraph>(peer->GetProfile(),300));
}

} // namespace

