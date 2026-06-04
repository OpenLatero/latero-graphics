#include "gratingmodulatorwidget.h"
#include "gratingmodulator.h"
#include "gtk/numwidget.h"

namespace latero::graphics {

class GratingModulatorModeDropDown : public Gtk::Box
{
public:
	GratingModulatorModeDropDown(GratingModulatorPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL),
		list_(Gtk::StringList::create({})),
		dropDown_(list_),
		peer_(peer)
	{
		for (const auto& op : peer->GetModes())
			list_->append(op.label);
		Glib::ustring target = peer->GetMode().label;
		for (guint i = 0; i < list_->get_n_items(); ++i)
			if (list_->get_string(i) == target) { dropDown_.set_selected(i); break; }
		dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &GratingModulatorModeDropDown::OnChange));
		append(dropDown_);
	}
	virtual ~GratingModulatorModeDropDown() {};
	sigc::signal<void()> SignalChanged() { return signalChanged_; };
private:
	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	sigc::signal<void()> signalChanged_;
	void OnChange() { peer_->SetMode(std::string(list_->get_string(dropDown_.get_selected()))); signalChanged_(); };
	GratingModulatorPtr peer_;
};


class GratingModulatorFactorWidget : public Gtk::Box
{
public:
	GratingModulatorFactorWidget(GratingModulatorPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetFactor(),0.1,10)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 1, units::none));
		adj_->signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorFactorWidget::OnChanged));
	}
	virtual ~GratingModulatorFactorWidget() {}
protected:
	void OnChanged() { peer_->SetFactor(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	GratingModulatorPtr peer_;
};

class GratingModulatorLengthWidget : public Gtk::Box
{
public:
	GratingModulatorLengthWidget(GratingModulatorPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetLength(),1,300)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 0, peer->GetUnits()));
		adj_->signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorLengthWidget::OnChanged));
	}
	virtual ~GratingModulatorLengthWidget() {}
protected:
	void OnChanged() { peer_->SetLength(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	GratingModulatorPtr peer_;
};

class GratingModulatorPosWidget : public Gtk::Box
{
public:
	GratingModulatorPosWidget(GratingModulatorPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL), adj_(Gtk::Adjustment::create(peer->GetPosition(),0,360)), peer_(peer)
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL,adj_,0,peer->GetUnits()));
		adj_->signal_value_changed().connect(
			sigc::mem_fun(*this, &GratingModulatorPosWidget::OnChanged));
	}
	virtual ~GratingModulatorPosWidget() {}
protected:
	void OnChanged() { peer_->SetPosition(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	GratingModulatorPtr peer_;
};


GratingModulatorCtrl::GratingModulatorCtrl(GratingModulatorPtr peer) :
	widgetbox_(Gtk::Orientation::HORIZONTAL), Gtk::Frame("pitch modulation"), peer_(peer)
{
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);

	auto modeDropDown = Gtk::make_managed<GratingModulatorModeDropDown>(peer);
	auto factorWidget = Gtk::make_managed<GratingModulatorFactorWidget>(peer);
	auto posWidget = Gtk::make_managed<GratingModulatorPosWidget>(peer);
	auto lengthWidget = Gtk::make_managed<GratingModulatorLengthWidget>(peer);

	widgetbox_.set_hexpand();
	factorWidget->set_hexpand();
	posWidget->set_hexpand();
	lengthWidget->set_hexpand();

	set_child(*box);
	box->append(check_);
	box->append(widgetbox_);
	widgetbox_.append(*modeDropDown);
	widgetbox_.append(*factorWidget);
	widgetbox_.append(*posWidget);
	widgetbox_.append(*lengthWidget);

	check_.set_active(peer_->GetEnable());
	widgetbox_.set_sensitive(check_.get_active());
	check_.signal_toggled().connect(sigc::mem_fun(*this, &GratingModulatorCtrl::OnClick));
}
    
void GratingModulatorCtrl::OnClick()
{
	peer_->SetEnable(check_.get_active());
	widgetbox_.set_sensitive(check_.get_active());
};

} // namespace
