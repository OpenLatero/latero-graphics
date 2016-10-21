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

#include "plot.h"
#include <assert.h>
#include <gtkmm.h>

namespace latero {
namespace graphics { 

namespace gtk {


class PlotSaveDlg : public Gtk::FileChooserDialog
{
public:
	PlotSaveDlg() :
		Gtk::FileChooserDialog("Please select file name.", Gtk::FILE_CHOOSER_ACTION_SAVE),
		wAdj_(1000, 0, 1000), hAdj_(500, 0, 1000)
	{
		std::string dir = Glib::get_current_dir();
		set_current_folder(dir);
		add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
		set_default_response(Gtk::RESPONSE_CANCEL);
		set_current_name("plot.svg");
	
		// TODO...
		get_vbox()->pack_start(*manage(new Gtk::SpinButton(wAdj_)));
		get_vbox()->pack_start(*manage(new Gtk::SpinButton(hAdj_)));
	}

	uint GetWidth() { return wAdj_.get_value(); }
	uint GetHeight() { return hAdj_.get_value(); }
protected:
	Gtk::Adjustment wAdj_, hAdj_;
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
}

Plot::~Plot()
{
}

void Plot::Clear()
{
	for (unsigned int i=0; i<channels_.size(); i++)
		channels_[i].points.clear();
	

}

void Plot::Draw()
{
	if (is_realized())
	{
		Gdk::Rectangle invRect(0, 0, get_allocation().get_width(), get_allocation().get_height());
		get_window()->invalidate_rect(invRect, false);
	}
}


bool Plot::on_expose_event(GdkEventExpose* event)
{
	Glib::RefPtr<Gdk::Window> window = get_window();
	if (!window) return true;

	Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
	if (event)
	{
		cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
    		cr->clip();
	}
	
	uint w = get_allocation().get_width();
	uint h = get_allocation().get_height();

	Draw(cr,w,h);
	return true;
}

void Plot::Draw(Cairo::RefPtr<Cairo::Context> cr, uint w, uint h, bool gtkmode)
{
	// clear background
	if (gtkmode)
		cr->set_source_rgb(1,1,1);
	else
		Gdk::Cairo::set_source_color(cr, bgColor_);
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
		Gdk::Cairo::set_source_color(cr, lineColor_);
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
			Gdk::Cairo::set_source_color(cr, channels_[c].color);
		if (points.size())
		{
			cr->move_to(points[0].x * w, points[0].y * h);
			for (unsigned int i=1; i<points.size(); i++)
				cr->line_to(points[i].x * w, points[i].y * 0.9 * h);
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
	const Gdk::Color* color = new Gdk::Color(fgColor);
	t.color = *color; 
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
	PlotSaveDlg dialog;
	if (Gtk::RESPONSE_OK == dialog.run())
	{
		SaveToFile(dialog.get_filename(), dialog.GetWidth(), dialog.GetHeight());
	}
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
		Cairo::RefPtr<Cairo::Surface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, w, h);
		Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
		Draw(cr,w,h,true);
		surface->write_to_png(filename);
	}
}

bool Plot::OnClick(GdkEventButton* event)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		((Gtk::Menu*)uiManager_->get_widget("/PopupMenu"))->popup(event->button, event->time);
		return true;
	}
	else
		return false;
}

void Plot::CreatePopupMenu()
{
	set_events(Gdk::BUTTON_PRESS_MASK);
	signal_button_press_event().connect(sigc::mem_fun(*this, &Plot::OnClick));

	Glib::RefPtr<Gtk::ActionGroup> group = Gtk::ActionGroup::create();
	group->add(Gtk::Action::create("ContextSave", "save as plot.svg/png", "save image"),
		sigc::mem_fun(*this, &Plot::OnSave));
	group->add(Gtk::Action::create("ContextSaveAs", "save as ", "save image"),
		sigc::mem_fun(*this, &Plot::OnSaveAs));


	uiManager_ = Gtk::UIManager::create();
	uiManager_->insert_action_group(group);

	std::stringstream buf;
	buf << "<ui>";
       	buf << "  <popup name='PopupMenu'>";
	buf << "     <menuitem action='ContextSave'/>";
	buf << "     <menuitem action='ContextSaveAs'/>";
       	buf << "  </popup>";
       	buf << "</ui>";
	uiManager_->add_ui_from_string(buf.str());
}


} // namespace gtk

} // namespace graphics
} // namespace latero
