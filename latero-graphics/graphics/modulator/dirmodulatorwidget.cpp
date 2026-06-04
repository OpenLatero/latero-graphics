#include "dirmodulatorwidget.h"
#include "dirmodulator.h"
#include "modulatorpreview.h"
#include "../../gtk/numwidget.h"

namespace latero::graphics {
namespace dir_modulator_ctrls {

class Ctrl : public Gtk::Box
{
public:
	Ctrl(DirModulatorPtr peer) : Gtk::Box(Gtk::Orientation::VERTICAL), peer_(peer) {}
	virtual ~Ctrl() {}
protected:
	DirModulatorPtr peer_;
};

class LowVelModeDropDown : public Ctrl
{
public:
	LowVelModeDropDown(DirModulatorPtr peer) : Ctrl(peer)
	{
		Gtk::Frame *frame = Gtk::make_managed<Gtk::Frame>("low velocity mode");
		append(*frame);
		modeList_ = Gtk::StringList::create({});
		std::vector<std::string> labels = peer->GetLowVelModeLabels();
		for (unsigned int i=0; i<labels.size(); ++i)
			modeList_->append(labels[i]);
		modeDropDown_ = Gtk::make_managed<Gtk::DropDown>(modeList_);
		Glib::ustring target = peer->GetLowVelModeLabel();
		for (guint i = 0; i < modeList_->get_n_items(); ++i)
			if (modeList_->get_string(i) == target) { modeDropDown_->set_selected(i); break; }
		modeDropDown_->property_selected().signal_changed().connect(sigc::mem_fun(*this, &LowVelModeDropDown::OnChange));
		frame->set_child(*modeDropDown_);
	}
	virtual ~LowVelModeDropDown() {};
	sigc::signal<void()> SignalChanged() { return signalChanged_; };
private:
	sigc::signal<void()> signalChanged_;
	void OnChange() { peer_->SetLowVelModeLabel(std::string(modeList_->get_string(modeDropDown_->get_selected()))); signalChanged_(); }
	Glib::RefPtr<Gtk::StringList> modeList_;
	Gtk::DropDown* modeDropDown_;
};


class DirCtrl : public Ctrl
{
public:
	DirCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetDirection(), 0, 360))
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL,adj_, 0, units::degree, "direction"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DirCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDirection(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class DirToleranceCtrl : public Ctrl
{
public:
	DirToleranceCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetDirTolerance(), 0, 180))
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 0, units::degree, "direction tolerance"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DirToleranceCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDirTolerance(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class DirTransitionCtrl : public Ctrl
{
public:
	DirTransitionCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetDirTransition(), 1, 180))
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 0, units::degree, "direction transition rate"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DirTransitionCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetDirTransition(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class VelToleranceCtrl : public Ctrl
{
public:
	VelToleranceCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetVelTolerance(), 0, 100))
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, adj_, 0, units::mm_per_sec, "velocity tolerance"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &VelToleranceCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetVelTolerance(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class VelTransitionCtrl : public Ctrl
{
public:
	VelTransitionCtrl(DirModulatorPtr peer) : Ctrl(peer), adj_(Gtk::Adjustment::create(peer->GetVelTransition(), 0, 100))
	{
		append(*Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL,adj_, 0, units::mm_per_sec, "velocity transition rate"));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &VelTransitionCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetVelTransition(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class SymmetricCtrl : public Ctrl
{
public:
	SymmetricCtrl(DirModulatorPtr peer) : Ctrl(peer), check_("symmetric")
	{
		Gtk::Frame *frame = Gtk::make_managed<Gtk::Frame>();
		append(*frame);
		frame->set_child(check_);
		check_.set_active(peer_->GetSymmetric());
		check_.signal_toggled().connect(sigc::mem_fun(*this, &SymmetricCtrl::OnClick));
	}
	void OnClick() { peer_->SetSymmetric(check_.get_active()); };
protected:
	Gtk::CheckButton check_;
};

} // namespace


DirModulatorWidget::DirModulatorWidget(DirModulatorPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	using namespace dir_modulator_ctrls;
	auto grid = Gtk::make_managed<Gtk::Grid>();
	grid->attach(*Gtk::make_managed<DirCtrl>(peer),0,0,1,1);
	grid->attach(*Gtk::make_managed<SymmetricCtrl>(peer),1,0,1,1);
	grid->attach(*Gtk::make_managed<LowVelModeDropDown>(peer),2,0,1,1);
	grid->attach(*Gtk::make_managed<DirToleranceCtrl>(peer),0,1,1,1);
	grid->attach(*Gtk::make_managed<DirTransitionCtrl>(peer),1,1,2,1);
	grid->attach(*Gtk::make_managed<VelToleranceCtrl>(peer),0,2,1,1);
	grid->attach(*Gtk::make_managed<VelTransitionCtrl>(peer),1,2,2,1);
	append(*grid);
	grid->set_hexpand();
	append(*Gtk::make_managed<ModulatorPreview>(peer));
}

} // namespace
