// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#include "pointwidget.h"
#include "gtk/numwidget.h"

namespace latero {
namespace graphics { 

PointWidget::PointWidget(const Point &init, double xlim_min, double xlim_max, double ylim_min, double ylim_max, bool showSliders)  :
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	xAdj_(Gtk::Adjustment::create(init.x, xlim_min, xlim_max)),
	yAdj_(Gtk::Adjustment::create(init.y, ylim_min, ylim_max))
{
	if (showSliders)
	{
		auto xWidget = Gtk::manage(new latero::graphics::gtk::HNumWidget(xAdj_, 3, "mm"));
		auto yWidget = Gtk::manage(new latero::graphics::gtk::HNumWidget(yAdj_, 3, "mm"));

		append(*xWidget);
		append(*yWidget);

		xWidget->set_hexpand();
		yWidget->set_hexpand();
	}
	else
	{
		auto xWidget = Gtk::manage(new Gtk::SpinButton(xAdj_,0,3));
		auto yWidget = Gtk::manage(new Gtk::SpinButton(yAdj_,0,3));

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

} // namespace graphics
} // namespace latero
