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

#include "pointlistwidget.h"

namespace latero {
namespace graphics {

class NumButton : public Gtk::Button
{
public:
	NumButton(std::string label, int id) : Gtk::Button(label), id_(id)
	{
		signal_clicked().connect(sigc::mem_fun(*this, &NumButton::OnClicked));
	}
	virtual ~NumButton() {};
	sigc::signal<void(int)> SignalClicked() { return signalClicked_; };
protected:
	void OnClicked() { signalClicked_(id_); }
	sigc::signal<void(int)> signalClicked_;
	int id_;
};


PointListWidget::PointListWidget(const std::vector<Point> &points) :
	box_(Gtk::Orientation::VERTICAL), pointBox_(Gtk::Orientation::VERTICAL), insertButton_("+")
{
	set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
	set_placement(Gtk::CornerType::TOP_RIGHT);
	set_child(box_);

	for (unsigned int i=0; i<points.size(); ++i)
		InsertPoint(points[i]);
	box_.append(pointBox_);
	pointBox_.set_vexpand();
	box_.append(insertButton_);
	insertButton_.signal_clicked().connect(sigc::mem_fun(*this, &PointListWidget::OnInsert));
}

PointListWidget::~PointListWidget()
{
	DeleteAdj();
};

void PointListWidget::InsertPoint(const Point &p)
{
    auto xAdj = Gtk::Adjustment::create(p.x,0,100000);
    auto yAdj = Gtk::Adjustment::create(p.y,0,100000);

	xAdj_.push_back(xAdj);
	yAdj_.push_back(yAdj);
	int i = static_cast<int>(xAdj_.size())-1;

	xAdj->signal_value_changed().connect(signalChanged_);
	yAdj->signal_value_changed().connect(signalChanged_);

	rowBox_.push_back(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
	pointBox_.append(*rowBox_[i]);

	auto xSpin = Gtk::make_managed<Gtk::SpinButton>(xAdj);
	auto ySpin = Gtk::make_managed<Gtk::SpinButton>(yAdj);

	xSpin->set_hexpand();
	ySpin->set_hexpand();

	rowBox_[i]->append(*xSpin);
	rowBox_[i]->append(*ySpin);

	NumButton *delButton = Gtk::make_managed<NumButton>("-", static_cast<int>(xAdj_.size())-1);
	rowBox_[i]->append(*delButton);
	delButton->SignalClicked().connect(sigc::mem_fun(*this, &PointListWidget::OnDelete));
}

void PointListWidget::OnDelete(int i)
{
	if (i < (int)xAdj_.size())
	{
		pointBox_.remove(*rowBox_[i]);
		delete rowBox_[i];
		rowBox_.erase(rowBox_.begin()+i);
		xAdj_.erase(xAdj_.begin()+i);
		yAdj_.erase(yAdj_.begin()+i);

		signalChanged_();
	}
}

void PointListWidget::OnInsert()
{
	InsertPoint(Point(0,0));
	signalChanged_();
}

void PointListWidget::DeleteAdj()
{
	xAdj_.erase(xAdj_.begin(), xAdj_.end());
	yAdj_.erase(yAdj_.begin(), yAdj_.end());

	for (unsigned int i=0; i<rowBox_.size(); ++i)
		delete rowBox_[i];
	rowBox_.erase(rowBox_.begin(), rowBox_.end());
}

std::vector<Point> PointListWidget::GetPoints()
{
	std::vector<Point> rv;
	for (unsigned int i=0; i<xAdj_.size(); ++i)
		rv.push_back(Point(xAdj_[i]->get_value(), yAdj_[i]->get_value()));
	return rv;
}

} // namespace graphics
} // namespace latero

