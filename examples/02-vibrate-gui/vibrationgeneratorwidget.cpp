#include "vibrationgeneratorwidget.h"

VibrationGeneratorWidget::VibrationGeneratorWidget(VibrationGeneratorPtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL, 6),
	peer_(peer)
{
	set_halign(Gtk::Align::CENTER);

	auto label = Gtk::make_managed<Gtk::Label>("Frequency (Hz)");
	append(*label);

	// Initialize the Gtk::Adjustment with the VibrationGenerator's frequency. The thread-safe GetFrequency() is used since 
	// we have two threads: GUI and TactileEngine.
	adj_ = Gtk::Adjustment::create(peer_->GetFrequency(), 0.0, 200.0, 0.1, 10.0);
	adj_->signal_value_changed().connect(sigc::mem_fun(*this, &VibrationGeneratorWidget::OnChanged));

	auto spin = Gtk::make_managed<Gtk::SpinButton>(adj_, 0.1, 1);
	append(*spin);
}

void VibrationGeneratorWidget::OnChanged()
{
	// Update the VibrationGenerator's frequency using the thread-safe SetFrequency().
	peer_->SetFrequency(adj_->get_value());
}
