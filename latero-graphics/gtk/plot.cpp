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

#include <filesystem>
#include "plot.h"
#include <assert.h>
#include <iostream>

namespace latero {
namespace graphics { 

namespace gtk {


class PlotSaveDlg : public Gtk::Window
{
public:
	sigc::signal<void(std::string, uint, uint)> signalSave;

	PlotSaveDlg() :
		wAdj_(Gtk::Adjustment::create(1000, 1, 10000, 10)),
		hAdj_(Gtk::Adjustment::create(500, 1, 10000, 10))
	{
		set_title("Save Plot");
		set_modal(true);
		set_resizable(false);

		auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
		box->set_margin(12);

		auto sizeBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
		sizeBox->append(*Gtk::make_managed<Gtk::Label>("Width:"));
		sizeBox->append(*Gtk::make_managed<Gtk::SpinButton>(wAdj_));
		sizeBox->append(*Gtk::make_managed<Gtk::Label>("Height:"));
		sizeBox->append(*Gtk::make_managed<Gtk::SpinButton>(hAdj_));
		box->append(*sizeBox);

		auto btnBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
		auto cancelBtn = Gtk::make_managed<Gtk::Button>("Cancel");
		auto saveBtn = Gtk::make_managed<Gtk::Button>("Save As...");
		btnBox->append(*cancelBtn);
		btnBox->append(*saveBtn);
		box->append(*btnBox);

		set_child(*box);

		cancelBtn->signal_clicked().connect([this]() { close(); });
		saveBtn->signal_clicked().connect([this]() { OnPickFile(); });
	}

protected:
	void OnPickFile()
	{
		auto dialog = Gtk::FileDialog::create();
		dialog->set_title("Save Plot");
		dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
		dialog->set_initial_name("plot.svg");
		dialog->save(*this, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
			try {
				auto file = dialog->save_finish(result);
				signalSave(file->get_path(), wAdj_->get_value(), hAdj_->get_value());
				close();
			} catch (const Gtk::DialogError&) {}
		});
	}

	Glib::RefPtr<Gtk::Adjustment> wAdj_, hAdj_;
};


Plot::Plot(const char *fgColor, const char *bgColor) :
	bgColor_(bgColor),
	lineColor_("gray"),
	xMin_(0.0),
	xMax_(70000),
	yMin_(-1.0),
	yMax_(1.0)
{
	// minimum of one channel present
	AddChannel(fgColor);
	CreatePopupMenu();

    set_draw_func(sigc::mem_fun(*this, &Plot::OnDraw));
}

Plot::~Plot()
{
	if (popupMenu_) popupMenu_->unparent();
}

void Plot::Clear()
{
	for (unsigned int i=0; i<channels_.size(); i++)
		channels_[i].points.clear();
	

}

void Plot::Draw()
{
	queue_draw();
}


void Plot::OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int w, int h)
{
	Draw(cr,w,h);
}

void Plot::Draw(Cairo::RefPtr<Cairo::Context> cr, int w, int h, bool gtkmode)
{
	// clear background
	if (gtkmode)
		cr->set_source_rgb(1,1,1);
	else
		Gdk::Cairo::set_source_rgba(cr, bgColor_);
	cr->rectangle(0, 0, w, h);
	cr->fill();

	// only draw up to 0.9h

	// invert so that +y is towards top
	cr->scale(1,-1);
	cr->translate(0,-h);

	// TODO: let cairo take care of the scaling...

	// draw middle axis
	cr->set_line_width(2.0);
	if (gtkmode)
		cr->set_source_rgb(1.0,1.0,1.0);
	else
		Gdk::Cairo::set_source_rgba(cr, lineColor_);
	cr->move_to(0, 0.45*h);
    cr->line_to(w, 0.45*h);
	cr->stroke();

	// draw each curve
	for(unsigned int c =0; c<channels_.size(); c++)
	{
		std::vector<Point> &points = channels_[c].points;
		if (gtkmode)
		{
			cr->set_line_width(10.0);
			cr->set_source_rgb(0,0,0);
		}
		else
			Gdk::Cairo::set_source_rgba(cr, channels_[c].color);

		if (points.size())
		{
			cr->move_to(points[0].x * w, points[0].y * 0.9 * h);
			for (unsigned int i=1; i<points.size(); i++) {
				cr->line_to(points[i].x * w, points[i].y * 0.9 * h);
			}
			cr->stroke();
		}
	}
}


void Plot::InsertPoint(float x, float y)
{
	InsertPoint(0, x, y);
}

void Plot::SetRangeX(float min, float max)
{
	xMin_ = min;
	xMax_ = max;
}

void Plot::SetRangeY(float min, float max)
{
	yMin_ = min;
	yMax_ = max;
}

void Plot::AddChannel(const char *fgColor)
{
	Trace t;
	t.color = Gdk::RGBA(fgColor);
	channels_.push_back(t);
}

void Plot::InsertPoint(unsigned int channel, float x, float y)
{
	assert(channel < channels_.size());

	Point scaledPoint;
	scaledPoint.x =	(x - xMin_)/(xMax_ - xMin_);
	scaledPoint.y = (y - yMin_)/(yMax_ - yMin_);
	channels_[channel].points.push_back(scaledPoint);
}

void Plot::OnSaveAs()
{
	auto* dialog = new PlotSaveDlg();
	if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
		dialog->set_transient_for(*win);
	dialog->signalSave.connect([this](std::string path, uint w, uint h) {
		SaveToFile(path, w, h);
	});
	dialog->set_visible();
}

void Plot::OnSave()
{
	SaveToFile("plot.svg", 1000, 500);
	SaveToFile("plot.png", 1000, 500);
}


void Plot::SaveToFile(std::string filename, uint w, uint h)
{
	std::string ext = filename.substr(filename.find_last_of(".")+1);
	if (ext == "svg")
	{
		Cairo::RefPtr<Cairo::Surface> surface = Cairo::SvgSurface::create(filename, w, h);
		Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
		Draw(cr,w,h,true);
	}
	else if (ext == "png")
	{
		Cairo::RefPtr<Cairo::Surface> surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, w, h);
		Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
		Draw(cr,w,h,true);
		surface->write_to_png(filename);
	}
}

void Plot::OnClick(int n_press, double x, double y)
{
	popupMenu_->set_pointing_to(Gdk::Rectangle(x, y, 1, 1));
	popupMenu_->popup();
}

void Plot::CreatePopupMenu()
{
	auto gesture = Gtk::GestureClick::create();
	gesture->set_button(GDK_BUTTON_SECONDARY);
	gesture->signal_pressed().connect(sigc::mem_fun(*this, &Plot::OnClick));
	add_controller(gesture);

	// Create action group and add actions
	auto action_group = Gio::SimpleActionGroup::create();
	action_group->add_action("save", sigc::mem_fun(*this, &Plot::OnSave));
	action_group->add_action("saveas", sigc::mem_fun(*this, &Plot::OnSaveAs));
	insert_action_group("plot", action_group);

	// Define the popup menu using Builder XML
	auto builder = Gtk::Builder::create_from_string(R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<interface>
  	<menu id="PopupMenu">
    	<item>
      		<attribute name="label">save as plot.svg/png</attribute>
      		<attribute name="action">plot.save</attribute>
    	</item>
    	<item>
      		<attribute name="label">save as</attribute>
      		<attribute name="action">plot.saveas</attribute>
    	</item>
  	</menu>
	</interface>
	)");

	// Get the menu and create a Gtk::Menu from it
	auto menu_model = std::dynamic_pointer_cast<Gio::MenuModel>(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::PopoverMenu>(menu_model);
	popupMenu_->set_parent(*this);
}


} // namespace gtk

} // namespace graphics
} // namespace latero
