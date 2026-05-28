#include "ridgewidget.h"
#include <iostream>
#include "gtk/numwidget.h"
#include "ridge.h"

namespace latero::graphics {

RidgeEdgeWidthScale::RidgeEdgeWidthScale(RidgePtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL),
	peer_(peer),
	adj_(Gtk::Adjustment::create(peer->GetEdgeWidth(), Ridge::edgeWidth_min, 10, 10, 50))
{
	append(*Gtk::make_managed<gtk::HNumWidget>("edge width", adj_, 2, "mm"));
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &RidgeEdgeWidthScale::OnChange));
}
void RidgeEdgeWidthScale::OnChange() { peer_->SetEdgeWidth(adj_->get_value()); }




class RidgeTxAmpScale : public Gtk::Box
{
public:
	RidgeTxAmpScale(RidgePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), peer_(peer), adj_(Gtk::Adjustment::create(100*peer->GetTxAmp(), 0, 100, 10, 50))
	{
		append(*Gtk::make_managed<gtk::HNumWidget>(adj_, 0, "%"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &RidgeTxAmpScale::OnChange));
	}
	virtual ~RidgeTxAmpScale() {};
private:
	void OnChange() { peer_->SetTxAmp(adj_->get_value()/100); };
	RidgePtr peer_;
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class RidgeTxNbCyclesScale : public Gtk::Box
{
public:
	RidgeTxNbCyclesScale(RidgePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), peer_(peer), adj_(Gtk::Adjustment::create(peer->GetTxNbCycles(),1,20.0))
	{
		append(*Gtk::make_managed<gtk::HNumWidget>(adj_, 0));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &RidgeTxNbCyclesScale::OnChange));
	}
	virtual ~RidgeTxNbCyclesScale() {};
private:
	void OnChange() { peer_->SetTxNbCycles((uint)adj_->get_value()); };
	RidgePtr peer_;
    Glib::RefPtr<Gtk::Adjustment> adj_;
};


RidgeTextureCtrl::RidgeTextureCtrl(RidgePtr peer) :
	gtk::CheckFrame(peer->GetTxEnable(), "texture"), peer_(peer)
{
	auto ridgeTxAmpScale = Gtk::make_managed<RidgeTxAmpScale>(peer);
	auto ridgeTxNbCyclesScale = Gtk::make_managed<RidgeTxNbCyclesScale>(peer);

	GetBox().append(*ridgeTxAmpScale);
	GetBox().append(*ridgeTxNbCyclesScale);

	ridgeTxAmpScale->set_hexpand();
	ridgeTxNbCyclesScale->set_hexpand();
	
	GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &RidgeTextureCtrl::OnClick));
}
void RidgeTextureCtrl::OnClick() { peer_->SetTxEnable(GetCheck().get_active()); };



RidgeControls::RidgeControls(RidgePtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL),
	peer_(peer),
	edgeWidthScale_(peer),
	txCtrl_(peer)
{
	set_size_request(300,-1);
	append(edgeWidthScale_);
	append(txCtrl_);

	edgeWidthScale_.set_vexpand();
	txCtrl_.set_vexpand();
}


///////////////////////////////////////

RidgeWidget::RidgeWidget(RidgePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	controls_(peer),
    peer_(peer)
{
	graph_ = Gtk::make_managed<RidgeGraph>(peer);
	append(controls_);
	append(*graph_);

	controls_.set_hexpand();
	graph_->set_hexpand();

	graph_->Refresh();
}

} // namespace

