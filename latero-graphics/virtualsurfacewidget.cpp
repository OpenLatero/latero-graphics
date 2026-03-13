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
#include "virtualsurfacewidget.h"
#include "gtk/pixbufops.h"
#include <gtkmm.h>
#include <math.h>
#include "generator.h"
#include "positiongen.h"
#include "visualizewidget.h"
#include <gtkmm/menu.h>
#include <gtkmm/filechooserdialog.h>
#include <iostream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define UPDATE_RATE_MS 300

namespace latero {
namespace graphics {
 
VirtualSurfaceArea::VirtualSurfaceArea(const latero::Tactograph *dev) :
	showCursor_(false), showBorder_(false), animateCursor_(true),
	dev_(dev),
	tdAngle_(0),
	tdState_(dev->GetFrameSizeX(), dev->GetFrameSizeY()),
	rounded_(false),
	disablePopup_(false)
{
	Clear(0xffffffff);

	// TODO: The following was removed to temporarily disable animation. This saves a lot
	// of processor time and makes the user interface much smoother.

	//anim_.Activate(UPDATE_RATE_MS);
	//anim_.signal_current_frame_changed.connect(
	//	sigc::mem_fun(*this, &VirtualSurfaceArea::Invalidate));

	CreatePopupMenu();
    
    if (dev->IsEmulated())
	{
		set_events(Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_MOTION_MASK|Gdk::BUTTON_RELEASE_MASK);
		signal_button_press_event().connect(sigc::mem_fun(*this, &VirtualSurfaceArea::OnButtonPress));
		signal_button_release_event().connect(sigc::mem_fun(*this, &VirtualSurfaceArea::OnButtonRelease));
		signal_motion_notify_event().connect(sigc::mem_fun(*this, &VirtualSurfaceArea::OnMotionNotify));
	}
    
    signal_draw().connect(sigc::mem_fun(*this, &VirtualSurfaceArea::OnDraw));
}

bool VirtualSurfaceArea::OnClick(GdkEventButton* event)
{
	if (disablePopup_) return false;
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		popupMenu_->popup_at_pointer((GdkEvent*)event);
		return true;
	}
	else
		return false;
}

void VirtualSurfaceArea::CreatePopupMenu()
{
	set_events(Gdk::BUTTON_PRESS_MASK);
	signal_button_press_event().connect(sigc::mem_fun(*this, &VirtualSurfaceArea::OnClick));

	// Create action group and add actions
	auto action_group = Gio::SimpleActionGroup::create();
	action_group->add_action("save", sigc::mem_fun(*this, &VirtualSurfaceArea::OnSave));
	insert_action_group("virtualsurface", action_group);

	// Define the popup menu using Builder XML
	auto builder = Gtk::Builder::create_from_string(R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<interface>
  		<menu id="PopupMenu">
    		<item>
      			<attribute name="label">save</attribute>
      			<attribute name="action">virtualsurface.save</attribute>
    		</item>
  		</menu>
	</interface>
	)");

	// Get the menu and create a Gtk::Menu from it
	auto menu_model = Glib::RefPtr<Gio::Menu>::cast_dynamic(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::Menu>(menu_model);
	popupMenu_->attach_to_widget(*this);
}

void VirtualSurfaceArea::OnSave()
{
	anim_.SaveToFile();
}



void VirtualSurfaceArea::SetRounded(bool v)
{
	rounded_ = v;
}

VirtualSurfaceArea::~VirtualSurfaceArea()
{
	anim_.Deactivate();
}

void VirtualSurfaceArea::on_size_allocate(Gtk::Allocation& allocation)
{
	Gtk::DrawingArea::on_size_allocate(allocation);
    
	// We can't resize the animation here: quality degrades too quickly.
	// We could keep a second resized copy, but that would require a lot of memory.
}

bool VirtualSurfaceArea::OnDraw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (rounded_)
	{
		DrawBorderPath(cr);
    	cr->clip();	
    }

    if (!anim_.GetNbFrames())
    {
    	cr->set_source_rgb(1.0, 1.0, 1.0);
    	cr->paint();
        return true;
    }

    //cr->save();
    //cr->set_source_rgb(1.0, 0.0, 0.0);
    //cr->paint();
    //cr->restore();
    //return true;

    Glib::RefPtr<Gdk::Pixbuf> buf = anim_.GetCurrentFrame();
    if (buf)
    {
        // the animation might not have the right size
        buf = buf->scale_simple(GetWidth(),GetHeight(),Gdk::INTERP_NEAREST);
        Gdk::Cairo::set_source_pixbuf(cr, buf, 0, 0);
        cr->paint();
    }

    if (showCursor_)    DrawCursor(cr);
    if (showBorder_)    DrawBorder(cr);

    return true;

}


Cairo::RefPtr<Cairo::Pattern> VirtualSurfaceArea::GetCursorDrawing(const Cairo::RefPtr<Cairo::Context> &cr)
{
	cr->push_group();

	/**
	 * @todo
	 * Use the actual position of the piezos used for computations rather
	 * than rotating the frame as done here. This will allow us to
	 * catch any mistakes...
	 */

	cr->scale(dpmm_x(), dpmm_y());		// scale to mm

	if ((GetWidth() < 300)||!animateCursor_) // TODO: find a good value
	{
		cr->translate(tdPos_.x, tdPos_.y);	// shift origin to center of TD
		cr->rotate(-tdAngle_);			// line up with TD

		cr->set_line_width(0.0);
		cr->set_source_rgba(1.0, 0.0, 0.0, 0.5);
		cr->rectangle(
			-dev_->GetWidth()/2.0,
			-dev_->GetHeight()/2.0,
			dev_->GetWidth(),
			dev_->GetHeight());
		cr->fill();
		cr->stroke();
	}
	else
	{
		cr->set_source(GetDisplayDrawing(cr));
		cr->paint();
	}
	return cr->pop_group();
}

Cairo::RefPtr<Cairo::Pattern>
VirtualSurfaceArea::GetDisplayDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext)
{
	mmContext->push_group();

	mmContext->translate(tdPos_.x, tdPos_.y);	// shift origin to center of TD
	mmContext->rotate(-tdAngle_);			// line up with TD

	double tdw = dev_->GetWidth()*1.4;
	double tdh = dev_->GetHeight()*1.2;
	mmContext->rectangle(-tdw/2, -tdh/2, tdw, tdh);
	mmContext->set_source_rgb(1.0, 1.0, 1.0);
	mmContext->set_line_width(1.5);
	mmContext->fill_preserve();
	mmContext->set_line_width(1.5);
	mmContext->set_source_rgb(1.0, 1.0, 1.0);
	mmContext->stroke_preserve();
	mmContext->set_line_width(0.5);
	mmContext->set_source_rgb(1.0, 0.0, 0.0);
	mmContext->stroke();

	float motionRange = 0.7 * dev_->GetPitchX();
	int hPiezo = dev_->GetContactorSizeY();

	/*
	mmContext->set_source_rgb(0.0, 0.0, 1.0);
	for (uint j=0; j< dev_->GetFrameSizeY(); ++j)
	{
		for (uint i=0; i< dev_->GetFrameSizeX(); ++i)
		{
			latero::graphics::Point p = dev_->GetActuatorOffset(i,j);
			float x = p.x;
			mmContext->move_to(x, p.y - 0.5*tdh);
	        	mmContext->line_to(x, p.y + 0.5*tdh);
			mmContext->set_line_width(0.2*dev_->GetPitchX());
			mmContext->stroke();
		}
	}
	*/


	mmContext->set_source_rgb(1.0, 0.0, 0.0);
	for (uint j=0; j< dev_->GetFrameSizeY(); ++j)
	{
		for (uint i=0; i< dev_->GetFrameSizeX(); ++i)
		{
			latero::graphics::Point p = dev_->GetActuatorOffset(i,j);
			float x = p.x + (0.5-tdState_.Get(i,j))*motionRange;
			mmContext->move_to(x, p.y - 0.5*hPiezo);
	        	mmContext->line_to(x, p.y + 0.5*hPiezo);

			//mmContext->set_source_rgb(1.0, 1.0, 1.0);
			//mmContext->set_line_width(0.8*dev_->GetPitchX());
			//mmContext->stroke_preserve();

			mmContext->set_source_rgb(1.0, 0.0, 0.0);
			mmContext->set_line_width(0.3*dev_->GetPitchX());
			mmContext->stroke();
		}
	}
	return mmContext->pop_group();
}


void VirtualSurfaceArea::DrawCursor(const Cairo::RefPtr<Cairo::Context> &cr)
{
	cr->set_source(GetCursorDrawing(cr));
	cr->paint();
}

void VirtualSurfaceArea::DrawBorderPath(const Cairo::RefPtr<Cairo::Context> &cr)
{
	int radius = 0.1 * GetWidth();

	int l = 0;
	int t = 0;
	int r = l + GetWidth();
	int b = t + GetHeight();

	cr->set_line_width(5.0);

	cr->move_to(l, b-radius);
	cr->arc(l + radius, t + radius, radius, M_PI, 1.5*M_PI);
	cr->arc(r - radius, t + radius, radius, 1.5*M_PI, 2.0*M_PI);
	cr->arc(r - radius, b - radius, radius, 0.0*M_PI, 0.5*M_PI);
	cr->arc(l + radius, b - radius, radius, 0.5*M_PI, 1.0*M_PI);
}


void VirtualSurfaceArea::DrawBorder(const Cairo::RefPtr<Cairo::Context> &cr)
{
	cr->save();
	cr->set_source_rgba(0.0, 0.0, 1.0, 1.0);
	DrawBorderPath(cr);
	cr->stroke();
	cr->restore();
}

Gdk::Rectangle VirtualSurfaceArea::GetDisplayFootprint(uint border)
{
	// to take into account any angle...
	double diam = sqrt(dev_->GetWidth()*dev_->GetWidth() + dev_->GetHeight()*dev_->GetHeight());

	float x0 = dpmm_x()*(tdPos_.x-diam/2.0) - border;
	float y0 = dpmm_y()*(tdPos_.y-diam/2.0) - border;
	float w = dpmm_x()*diam + 2*border;
	float h = dpmm_y()*diam + 2*border;
	
	Gdk::Rectangle rv;
	rv.set_x((int)floor(x0));
 	rv.set_y((int)floor(y0));
	rv.set_width((int)ceil(w));
	rv.set_height((int)ceil(h));

	return rv;
}

// TODO: pass State???
void VirtualSurfaceArea::SetDisplayState(const Point &pos, double angle, const latero::BiasedImg &f)
{
	assert(f.Size() ==  tdState_.Size());

	Gdk::Rectangle oldBox = GetDisplayFootprint(20);
	if (std::isnan(angle)) angle = 0; // for some reason angle is sometimes nan
	tdAngle_ = angle;
	tdPos_ = pos;
	tdState_ = f;
	Gdk::Rectangle newBox = GetDisplayFootprint(20);

	Gdk::Rectangle invRect(oldBox);
	invRect.join(newBox);

	Glib::RefPtr<Gdk::Window> win = get_window();
	if (win) win->invalidate_rect(invRect, false);
}

void VirtualSurfaceArea::Clear(guint32 pixel)
{
	Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create(
			Gdk::COLORSPACE_RGB, true, 8,
			GetWidth(), GetHeight());
	buf->fill(pixel);
	Set(buf);
}

void VirtualSurfaceArea::ShowCursor(bool v)
{
	if (showCursor_ != v)
	{
		showCursor_ = v;
		Invalidate();
	}
}

void VirtualSurfaceArea::ShowBorder(bool v)
{
	if (showBorder_ != v)
	{
		showBorder_ = v;
		Invalidate();
	}
}

void VirtualSurfaceArea::Invalidate()
{
    Glib::RefPtr<Gdk::Window> win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
}


void VirtualSurfaceArea::Set(latero::graphics::gtk::Animation &anim)
{
	std::cout << "VirtualSurfaceArea::Set(anim)\n";
	anim_ = anim;
	Invalidate();
}

void VirtualSurfaceArea::Set(Glib::RefPtr<Gdk::Pixbuf> buf)
{
	std::cout << "VirtualSurfaceArea::Set(pixbuf)\n";
	//std::stringstream stm;
	//struct timeval tv;
	//struct tm* ptm;
	//char time_string[40];
	//long milliseconds;
	//gettimeofday (&tv, NULL);
	//ptm = localtime (&tv.tv_sec);
	//strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	//stm << time_string << ".png";
	//buf->save(stm.str(), "png");
	latero::graphics::gtk::Animation v(buf);
	Set(v);
}


latero::graphics::gtk::Animation VirtualSurfaceArea::GetIllustration()
{
	return anim_;
}

void VirtualSurfaceArea::AnimateCursor(bool v)
{
	animateCursor_ = v;
}
    
Point VirtualSurfaceArea::GetClickPos(double x, double y)
{
    return Point(x * dev_->GetSurfaceWidth() / GetWidth(), y * dev_->GetSurfaceHeight() / GetHeight());
}
    
bool VirtualSurfaceArea::OnButtonPress(GdkEventButton* event)
{
	std::cout << "VirtualSurfaceArea::OnButtonPress\n";
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1))
        dev_->SetEmulatedState(GetClickPos(event->x, event->y));
    return false;
} 
    
bool VirtualSurfaceArea::OnMotionNotify(GdkEventMotion *event)
{
    if (event->state & GDK_BUTTON1_MASK)
        dev_->SetEmulatedState(GetClickPos(event->x, event->y));
    return true;
} 
    
bool VirtualSurfaceArea::OnButtonRelease(GdkEventButton* event)
{
    dev_->SetEmulatedState(GetClickPos(event->x, event->y));
    return true;
}

VirtualSurfaceWidget::VirtualSurfaceWidget(const latero::Tactograph *dev, GeneratorPtr gen, bool refreshBackground) :
	BaseVirtualSurfaceWidget(dev),
	peer_(gen)
{
	// TODO: enable these timeouts only when visible?!?
	// TODO: when that's done, make sure everything 2D uses this version (e.g. Memory game)

	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &VirtualSurfaceWidget::RefreshCursor),
		(uint)33, // ms
		Glib::PRIORITY_DEFAULT_IDLE);

	if (refreshBackground)
	{
		Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &VirtualSurfaceWidget::OnCheckPeer),
			(uint)333, // ms
			Glib::PRIORITY_DEFAULT_IDLE);
	}

	surface_.DisablePopup();
	CreatePopupMenu();
}

void VirtualSurfaceWidget::CreatePopupMenu()
{
	set_events(Gdk::EventMask::BUTTON_PRESS_MASK);
	signal_button_press_event().connect(sigc::mem_fun(*this, &VirtualSurfaceWidget::OnClick));

	// Create action group and add actions
	auto action_group = Gio::SimpleActionGroup::create();
	action_group->add_action("save",     sigc::mem_fun(*this, &VirtualSurfaceWidget::OnSave));
	action_group->add_action("visualize",sigc::mem_fun(*this, &VirtualSurfaceWidget::OnVisualize));
	action_group->add_action("refresh",  sigc::mem_fun(*this, &VirtualSurfaceWidget::RefreshBackground));
	action_group->add_action("edit",     sigc::mem_fun(*this, &VirtualSurfaceWidget::OnEdit));
	action_group->add_action("savecanvas",   sigc::mem_fun(*this, &VirtualSurfaceWidget::OnSaveCanvas));
	action_group->add_action("savecanvases", sigc::mem_fun(*this, &VirtualSurfaceWidget::OnSaveCanvasAs));
	insert_action_group("vsw", action_group);

	// Define the popup menu using Builder XML
	auto builder = Gtk::Builder::create_from_string(R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<interface>
  	<menu id="PopupMenu">
    	<item>
      	<attribute name="label">save image</attribute>
      	<attribute name="action">vsw.save</attribute>
    	</item>
    	<item>
      	<attribute name="label">visualize</attribute>
      	<attribute name="action">vsw.visualize</attribute>
    	</item>
    	<item>
      	<attribute name="label">refresh</attribute>
      	<attribute name="action">vsw.refresh</attribute>
    	</item>
    	<item>
      	<attribute name="label">edit canvas</attribute>
      	<attribute name="action">vsw.edit</attribute>
    	</item>
    	<item>
      	<attribute name="label">save canvas as</attribute>
      	<attribute name="action">vsw.savecanvases</attribute>
    	</item>
    	<item>
      	<attribute name="label">save canvas</attribute>
      	<attribute name="action">vsw.savecanvas</attribute>
    	</item>
  	</menu>
	</interface>
	)");

	// Get the menu and create a Gtk::Menu from it
	auto menu_model = Glib::RefPtr<Gio::Menu>::cast_dynamic(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::Menu>(menu_model);
	popupMenu_->attach_to_widget(*this);
}

bool VirtualSurfaceWidget::OnClick(GdkEventButton* event)
{
	std::cout << "VirtualSurfaceWidget::OnClick\n";
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		popupMenu_->popup_at_pointer((GdkEvent*)event);
		return true;
	}
	else
		return false;
}

void VirtualSurfaceWidget::OnSave()
{
	surface_.GetIllustration().SaveToFile();
}

void VirtualSurfaceWidget::OnVisualize()
{
	if (peer_)
	{
		PositionGenPtr gen = boost::dynamic_pointer_cast<PositionGen>(peer_);
		if (gen)
		{
			VisualizeWidget dlg(gen);
			dlg.run();
		}
	}
}

void VirtualSurfaceWidget::OnEdit()
{
	if (peer_)
	{
		Gtk::Dialog dlg;
		dlg.get_vbox()->pack_start(*manage(peer_->CreateWidget(peer_)));
		dlg.show_all_children();
		dlg.run();
	}
}

void VirtualSurfaceWidget::OnSaveCanvasAs()
{
	if (peer_)
	{
		Gtk::FileChooserDialog dialog("Please select a generator file.", Gtk::FILE_CHOOSER_ACTION_SAVE);
		std::string dir = std::filesystem::current_path().string();
		dialog.set_current_folder(dir);
		dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
		dialog.add_button("Save", Gtk::RESPONSE_OK);
		dialog.set_default_response(Gtk::RESPONSE_CANCEL);
		dialog.set_current_name("test.gen");
		if (Gtk::RESPONSE_OK == dialog.run())		
		{
			std::string filename = dialog.get_filename();
			peer_->SaveToFile(filename);
		}
	}
}

void VirtualSurfaceWidget::OnSaveCanvas()
{
	if (peer_) peer_->SaveToFile("canvas.gen");
}


VirtualSurfaceWidget::~VirtualSurfaceWidget()
{
}

bool VirtualSurfaceWidget::RefreshCursor()
{
	if (peer_)
	{
		Point pos = peer_->GetDisplayCenter();
		double angle = peer_->GetDisplayOrientation();
		latero::BiasedImg frame = peer_->GetLatestFrame();
		SetDisplayState(pos, angle, frame);
	}
	return true;
}


bool VirtualSurfaceWidget::OnCheckPeer()
{
	if (peer_)
		if (peer_->GetLastModified() > bgUpdateTime_)
			RefreshBackground();
	return true;
}


void VirtualSurfaceWidget::RefreshBackground()
{
	bgUpdateTime_ = boost::posix_time::microsec_clock::universal_time();
	if (peer_)
	{
		latero::graphics::gtk::Animation anim = peer_->GetIllustration(GetWidth(), boost::posix_time::seconds(0));
		Set(anim);
	}
}


void VirtualSurfaceWidget::on_size_allocate(Gtk::Allocation& allocation)
{
	BaseVirtualSurfaceWidget::on_size_allocate(allocation);
	RefreshBackground();
}

void VirtualSurfaceWidget::SetGenerator(GeneratorPtr gen)
{
	peer_ = gen;
	//RefreshBackground();
}

Point VirtualSurfaceWidget::GetDisplayPos()
{
	return surface_.GetDisplayPos();
}


} // namespace graphics
} // namespace latero

