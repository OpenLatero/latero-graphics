#include "pointwidget.h"
#include "gtk/numwidget.h"

namespace latero::graphics {

PointWidget::PointWidget(const Point &init, double xlim_min, double xlim_max, double ylim_min, double ylim_max, bool showSliders)  :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	xAdj_(Gtk::Adjustment::create(init.x, xlim_min, xlim_max)),
	yAdj_(Gtk::Adjustment::create(init.y, ylim_min, ylim_max))
{
	if (showSliders)
	{
		auto xWidget = Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, xAdj_, 3, "mm");
		auto yWidget = Gtk::make_managed<gtk::NumWidget>(Gtk::Orientation::HORIZONTAL, yAdj_, 3, "mm");

		append(*xWidget);
		append(*yWidget);

		xWidget->set_hexpand();
		yWidget->set_hexpand();
	}
	else
	{
		auto xWidget = Gtk::make_managed<Gtk::SpinButton>(xAdj_,0,3);
		auto yWidget = Gtk::make_managed<Gtk::SpinButton>(yAdj_,0,3);

		append(*xWidget);
		append(*yWidget);

		xWidget->set_hexpand();
		yWidget->set_hexpand();		
	}

	xAdj_->signal_value_changed().connect(signalValueChanged_);
	yAdj_->signal_value_changed().connect(signalValueChanged_);
}

Point PointWidget::GetValue()
{
	return Point(xAdj_->get_value(),yAdj_->get_value());
}

void PointWidget::SetValue(const Point &p)
{
	xAdj_->set_value(p.x);
	yAdj_->set_value(p.y);
}

sigc::signal<void()> PointWidget::SignalValueChanged()
{
	return signalValueChanged_;
}

} // namespace
