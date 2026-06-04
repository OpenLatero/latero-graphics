#include "closedpatternwidget.h"
#include "closedpattern.h"
#include "../gtk/numwidget.h"

namespace latero::graphics {

class ClosedPatternFillTextureGapCtrl : public Gtk::Box
{
public:
	ClosedPatternFillTextureGapCtrl(ClosedPatternPtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetFillTextureGap(), -1, 50)), peer_(peer)
	{
		append(*Gtk::make_managed<latero::graphics::gtk::NumWidget>(Gtk::Orientation::HORIZONTAL,adj_, 1, "mm", "gap"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureGapCtrl::OnChanged));
	}
	virtual ~ClosedPatternFillTextureGapCtrl() {};
protected:
	void OnChanged() { peer_->SetFillTextureGap(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	ClosedPatternPtr peer_;
};

class ClosedPatternFillTextureEdgeSizeCtrl : public Gtk::Box
{
public:
	ClosedPatternFillTextureEdgeSizeCtrl(ClosedPatternPtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetFillTextureEdgeSize(), 0, 50)), peer_(peer)
	{
		append(*Gtk::make_managed<latero::graphics::gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 1, "mm", "edge size"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureEdgeSizeCtrl::OnChanged));
	}
	virtual ~ClosedPatternFillTextureEdgeSizeCtrl() {};
protected:
	void OnChanged() { peer_->SetFillTextureEdgeSize(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;	
	ClosedPatternPtr peer_;
};





ClosedPatternFillTextureWidget::ClosedPatternFillTextureWidget(ClosedPatternPtr peer) :
	gtk::CheckFrame(peer->GetFillTextureEnable()), txWidget_(peer->GetFillTexture()), peer_(peer)
{
	auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto gapCtrl = Gtk::make_managed<ClosedPatternFillTextureGapCtrl>(peer);
	auto edgeSizeCtrl = Gtk::make_managed<ClosedPatternFillTextureEdgeSizeCtrl>(peer);

	vbox->set_hexpand();
	gapCtrl->set_hexpand();
	edgeSizeCtrl->set_hexpand();
	txWidget_.set_vexpand();

	GetBox().append(*vbox);
	auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	hbox->append(*gapCtrl);	
	hbox->append(*edgeSizeCtrl);
	vbox->append(*hbox);
	vbox->append(txWidget_);

	txWidget_.SignalTextureChanged().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureWidget::OnTextureChanged));;
	GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &ClosedPatternFillTextureWidget::OnCheck));
}
void ClosedPatternFillTextureWidget::OnCheck() { peer_->SetFillTextureEnable(GetCheck().get_active()); }
void ClosedPatternFillTextureWidget::OnTextureChanged() { peer_->SetFillTexture(txWidget_.GetTexture()); }


} // namespace

