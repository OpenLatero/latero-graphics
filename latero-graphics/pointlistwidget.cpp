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
	sigc::signal<void,int> SignalClicked() { return signalClicked_; };
protected:
	void OnClicked() { signalClicked_(id_); }
	sigc::signal<void,int> signalClicked_;
	int id_;
};


PointListWidget::PointListWidget(const std::vector<Point> &points) :
	insertButton_("+")
{
	set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	set_placement(Gtk::CORNER_TOP_RIGHT);
	add(box_);

	for (unsigned int i=0; i<points.size(); ++i)
		InsertPoint(points[i]);
	box_.pack_start(pointBox_);
	box_.pack_start(insertButton_, Gtk::PACK_SHRINK);
	insertButton_.signal_clicked().connect(sigc::mem_fun(*this, &PointListWidget::OnInsert));

	show_all_children();
}

PointListWidget::~PointListWidget()
{
	DeleteAdj();
};

void PointListWidget::InsertPoint(const Point &p)
{
	Gtk::Adjustment *xAdj = new Gtk::Adjustment(p.x,0,100000);
	Gtk::Adjustment *yAdj = new Gtk::Adjustment(p.y,0,100000);

	xAdj_.push_back(xAdj);
	yAdj_.push_back(yAdj);
	int i = xAdj_.size()-1;

	xAdj->signal_value_changed().connect(signalChanged_);
	yAdj->signal_value_changed().connect(signalChanged_);

	rowBox_.push_back(new Gtk::HBox);
	pointBox_.pack_start(*rowBox_[i], Gtk::PACK_SHRINK);
	rowBox_[i]->pack_start(*manage(new Gtk::SpinButton(*xAdj)));
	rowBox_[i]->pack_start(*manage(new Gtk::SpinButton(*yAdj)));

	NumButton *delButton = manage(new NumButton("-", xAdj_.size()-1));
	rowBox_[i]->pack_start(*delButton, Gtk::PACK_SHRINK);
	delButton->SignalClicked().connect(sigc::mem_fun(*this, &PointListWidget::OnDelete));
}

void PointListWidget::OnDelete(int i)
{
	if (i < (int)xAdj_.size())
	{
		pointBox_.remove(*rowBox_[i]);
		delete rowBox_[i];
		rowBox_.erase(rowBox_.begin()+i);

		delete xAdj_[i];
		xAdj_.erase(xAdj_.begin()+i);

		delete yAdj_[i];
		yAdj_.erase(yAdj_.begin()+i);

		show_all_children();
		signalChanged_();
	}
}

void PointListWidget::OnInsert()
{
	InsertPoint(Point(0,0));
	show_all_children();
	signalChanged_();
}

void PointListWidget::DeleteAdj()
{
	for (unsigned int i=0; i<xAdj_.size(); ++i)
		delete xAdj_[i];
	xAdj_.erase(xAdj_.begin(), xAdj_.end());

	for (unsigned int i=0; i<yAdj_.size(); ++i)
		delete yAdj_[i];
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

