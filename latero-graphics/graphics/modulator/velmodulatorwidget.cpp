#include "velmodulatorwidget.h"
#include "velmodulator.h"
#include "modulatorpreview.h"
#include "../../gtk/numwidget.h"

namespace latero::graphics {
namespace vel_modulator_ctrls {

class Ctrl : public Gtk::Box
{
public:
	Ctrl(VelModulatorPtr peer) : Gtk::Box(Gtk::Orientation::VERTICAL), peer_(peer) {}
	virtual ~Ctrl() {}
protected:
	VelModulatorPtr peer_;
};

class LimitCtrl : public Ctrl
{
public:
	LimitCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetLimit(), 0, 100))
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 0, units::mm_per_sec, "lower limit"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &LimitCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetLimit(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class TransitionCtrl : public Ctrl
{
public:
	TransitionCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetTransition(), 0, 100))
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 0, units::mm_per_sec, "transition rate"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &TransitionCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetTransition(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class DelayCtrl : public Ctrl
{
public:
	DelayCtrl(VelModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetDelay(), 0, 2000))
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 0, units::ms, "delayed onset"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DelayCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDelay(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};


} // namespace vel_modulator_ctrls


VelModulatorWidget::VelModulatorWidget(VelModulatorPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	using namespace vel_modulator_ctrls;

	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto limitCtrl = Gtk::make_managed<LimitCtrl>(peer);
	auto transitionCtrl = Gtk::make_managed<TransitionCtrl>(peer);
	auto delayCtrl = Gtk::make_managed<DelayCtrl>(peer);
	auto previewCtrl = Gtk::make_managed<ModulatorPreview>(peer);

	limitCtrl->set_vexpand();
	transitionCtrl->set_vexpand();
	delayCtrl->set_vexpand();
	box->set_hexpand();

	box->append(*limitCtrl);
	box->append(*transitionCtrl);
	box->append(*delayCtrl);

	append(*box);
	append(*previewCtrl);
}

} // namespace
