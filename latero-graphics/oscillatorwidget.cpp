#include "oscillatorwidget.h"
#include "oscillator.h"
#include "gtk/numwidget.h"

namespace latero::graphics {

class OscillatorBlendModeDropDown : public Gtk::Box
{
public:
	OscillatorBlendModeDropDown(OscillatorPtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL),
		list_(Gtk::StringList::create({})),
		dropDown_(list_),
		peer_(peer)
	{
		for (const auto& op : peer->GetBlendModes())
			list_->append(op.label);
		Glib::ustring target = peer->GetBlendMode().label;
		for (guint i = 0; i < list_->get_n_items(); ++i)
			if (list_->get_string(i) == target) { dropDown_.set_selected(i); break; }
		dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &OscillatorBlendModeDropDown::OnChange));
		append(dropDown_);
	};
	virtual ~OscillatorBlendModeDropDown() {};
private:
	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	void OnChange() { peer_->SetBlendMode(std::string(list_->get_string(dropDown_.get_selected()))); };
	OscillatorPtr peer_;
};


OscillatorEnableCheck::OscillatorEnableCheck(OscillatorPtr peer) :
	Gtk::CheckButton("vibrate"), peer_(peer)
{
	set_active(peer->GetEnable());
	signal_toggled().connect([this]{ peer_->SetEnable(get_active()); });
}


OscillatorAmplitudeCtrl::OscillatorAmplitudeCtrl(OscillatorPtr peer) :
    Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetAmplitude()*100,0,100)), peer_(peer)
{
	auto widget = Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL,adj_,0, units::percent);
	append(*widget);
	widget->set_vexpand();
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &OscillatorAmplitudeCtrl::OnChanged));
}
void OscillatorAmplitudeCtrl::OnChanged() { peer_->SetAmplitude(adj_->get_value()/100); };


OscillatorFreqCtrl::OscillatorFreqCtrl(OscillatorPtr peer) :
    Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetFreq(),0.1,50)), peer_(peer)
{
	auto widget = Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL,adj_,0, units::hz);
	append(*widget);
	widget->set_vexpand();
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &OscillatorFreqCtrl::OnChanged));
}
void OscillatorFreqCtrl::OnChanged() { peer_->SetFreq(adj_->get_value()); };


OscillatorWidget::OscillatorWidget(OscillatorPtr peer, bool showBlendBode) :
	gtk::CheckFrame(peer->GetEnable(), "vibration"), peer_(peer)
{
	auto amplitudeCtrl = Gtk::make_managed<OscillatorAmplitudeCtrl>(peer);
	auto freqCtrl = Gtk::make_managed<OscillatorFreqCtrl>(peer);

	amplitudeCtrl->set_hexpand();
	freqCtrl->set_hexpand();

	GetBox().append(*amplitudeCtrl);
	GetBox().append(*freqCtrl);

	if (showBlendBode) GetBox().append(*Gtk::make_managed<OscillatorBlendModeDropDown>(peer));
	GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &OscillatorWidget::OnClick));
}
void OscillatorWidget::OnClick() { peer_->SetEnable(GetCheck().get_active()); }

} // namespace
