#pragma once

#include <gtkmm.h>
#include "point.h"

namespace latero::graphics {

class PointListWidget : public Gtk::ScrolledWindow
{
public:
	PointListWidget(const std::vector<Point> &points);
	virtual ~PointListWidget();
	sigc::signal<void()> SignalChanged() { return signalChanged_; };
	std::vector<Point> GetPoints();
protected:
	void InsertPoint(const Point &p);
	void OnDelete(int i);
	void OnInsert();
	void DeleteAdj();

	sigc::signal<void()> signalChanged_;
	std::vector<Glib::RefPtr<Gtk::Adjustment>> xAdj_, yAdj_;
	std::vector<Gtk::Box*> rowBox_;
	Gtk::Box box_, pointBox_;
	Gtk::Button insertButton_;
};

} // namespace
