#include "dotpatternwidget.h"
#include "dotpattern.h"
#include "../gtk/numwidget.h"
#include "../ridgewidget.h"
#include "../pointwidget.h"
#include "../oscillatorwidget.h"
#include "../pointlistwidget.h"
#include "dotswidget.h"

namespace latero::graphics {

DotPatternMinSpacingCtrl::DotPatternMinSpacingCtrl(DotPatternPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetMinSpacing(), 0, 50.0)), peer_(peer)
{
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DotPatternMinSpacingCtrl::OnChanged));
	append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL,adj_,1,"minimal spacing","mm"));
}
void DotPatternMinSpacingCtrl::OnChanged() { peer_->SetMinSpacing(adj_->get_value()); }

DotPatternWidget::DotPatternWidget(DotPatternPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL)
{
	auto grid = Gtk::make_managed<Gtk::Grid>();
	grid->attach(*Gtk::make_managed<DotPatternMinSpacingCtrl>(peer),0,0,1,1);
	grid->attach(*Gtk::make_managed<DotsHeightCtrl>(peer),0,1,1,1);
	grid->attach(*Gtk::make_managed<RidgeEdgeWidthScale>(peer->GetProfile()),1,1,1,1);
	grid->attach(*Gtk::make_managed<RidgeTextureCtrl>(peer->GetProfile()),0,2,2,1);
	grid->attach(*Gtk::make_managed<OscillatorWidget>(peer->GetOscillator()),0,3,2,1);
	append(*grid);
	grid->set_hexpand();
	append(*Gtk::make_managed<RidgeGraph>(peer->GetProfile(),300));
}

} // namespace

