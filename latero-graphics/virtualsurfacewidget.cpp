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
}

bool VirtualSurfaceArea::OnClick(GdkEventButton* event)
{
	if (disablePopup_) return false;
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		((Gtk::Menu*)uiManager_->get_widget("/PopupMenu"))->popup(event->button, event->time);
		return true;
	}
	else
		return false;
}

void VirtualSurfaceArea::CreatePopupMenu()
{
	set_events(Gdk::BUTTON_PRESS_MASK);
	signal_button_press_event().connect(sigc::mem_fun(*this, &VirtualSurfaceArea::OnClick));

	Glib::RefPtr<Gtk::ActionGroup> group = Gtk::ActionGroup::create();
	group->add(Gtk::Action::create("ContextSave", "save", "save image"),
		sigc::mem_fun(*this, &VirtualSurfaceArea::OnSave));

	uiManager_ = Gtk::UIManager::create();
	uiManager_->insert_action_group(group);

	std::stringstream buf;
	buf << "<ui>";
       	buf << "  <popup name='PopupMenu'>";
	buf << "     <menuitem action='ContextSave'/>";
       	buf << "  </popup>";
       	buf << "</ui>";
	uiManager_->add_ui_from_string(buf.str());
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

Glib::RefPtr<Gdk::Bitmap> VirtualSurfaceArea::GetShapeCombineMask()
{
	Glib::RefPtr<Gdk::Bitmap> bitmap =
		Gdk::Bitmap::create("NULL", GetWidth(), GetHeight());
	Cairo::RefPtr<Cairo::Context> cr = bitmap->create_cairo_context();
	cr->set_operator(Cairo::OPERATOR_CLEAR);
	cr->paint();
	cr->set_operator(Cairo::OPERATOR_SOURCE);
	DrawBorderPath(cr);
	cr->fill();
	return bitmap;
}

void VirtualSurfaceArea::on_size_allocate(Gtk::Allocation& allocation)
{
	Gtk::DrawingArea::on_size_allocate(allocation);

	// this makes the corners transparent
	if (rounded_)
		shape_combine_mask(GetShapeCombineMask(),0,0);

	// We can't resize the animation here: quality degrades too quickly.
	// We could keep a second resized copy, but that would require a lot of memory.
}

bool VirtualSurfaceArea::on_expose_event(GdkEventExpose* event)
{
	if (!anim_.GetNbFrames())
	{
		get_window()->clear();
		return true;
	}

	Cairo::RefPtr<Cairo::Context> cr = get_window()->create_cairo_context();
	if (event)
	{
		cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
    		cr->clip();
	}

	/*
	cr->save();
	cr->set_source_rgb(1.0, 0.0, 0.0);
	cr->paint();
	cr->restore();
	return true;
	*/

	Glib::RefPtr<Gdk::Pixbuf> buf = anim_.GetCurrentFrame();
	if (buf)
	{
		/* 
		// this would allow Cairo to clip the image but it is much slower than render_to_drawable
		Gdk::Cairo::set_source_pixbuf (cr, buf, PhysicalToCanvasX(0), PhysicalToCanvasY(0));
		cr->paint();
		*/

		// the animation might not have the right size
		buf = buf->scale_simple(GetWidth(),GetHeight(),Gdk::INTERP_NEAREST);
		buf->render_to_drawable(get_window(), get_style()->get_black_gc(),
   			0, 0, 0, 0, GetWidth(), GetHeight(), Gdk::RGB_DITHER_NONE, 0, 0);
	}

	if (showCursor_)	DrawCursor(cr);
	if (showBorder_)	DrawBorder(cr);

	return true;
}

Cairo::RefPtr<Cairo::Pattern> VirtualSurfaceArea::GetCursorDrawing(Cairo::RefPtr<Cairo::Context> &cr)
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
VirtualSurfaceArea::GetDisplayDrawing(Cairo::RefPtr<Cairo::Context> &mmContext)
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


void VirtualSurfaceArea::DrawCursor(Cairo::RefPtr<Cairo::Context> &cr)
{
	cr->set_source(GetCursorDrawing(cr));
	cr->paint();
}

void VirtualSurfaceArea::DrawBorderPath(Cairo::RefPtr<Cairo::Context> &cr)
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


void VirtualSurfaceArea::DrawBorder(Cairo::RefPtr<Cairo::Context> &cr)
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
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1))
        dev_->SetEmulatedState(GetClickPos(event->x, event->y));
    return true;
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
	set_events(Gdk::BUTTON_PRESS_MASK);
	signal_button_press_event().connect(sigc::mem_fun(*this, &VirtualSurfaceWidget::OnClick));

	Glib::RefPtr<Gtk::ActionGroup> group = Gtk::ActionGroup::create();
	group->add(Gtk::Action::create("ContextSave", "save image", "save image"),
		sigc::mem_fun(*this, &VirtualSurfaceWidget::OnSave));
	group->add(Gtk::Action::create("ContextVisualize", "visualize", "visualize tactile image"),
		sigc::mem_fun(*this, &VirtualSurfaceWidget::OnVisualize));
	group->add(Gtk::Action::create("ContextRefresh", "refresh", "refresh image"),
		sigc::mem_fun(*this, &VirtualSurfaceWidget::RefreshBackground));
	group->add(Gtk::Action::create("ContextCanvasEdit", "edit canvas", "edit generator"),
		sigc::mem_fun(*this, &VirtualSurfaceWidget::OnEdit));
	group->add(Gtk::Action::create("ContextCanvasSave", "save canvas", "edit generator"),
		sigc::mem_fun(*this, &VirtualSurfaceWidget::OnSaveCanvas));
	group->add(Gtk::Action::create("ContextCanvasSaveAs", "save canvas as", "edit generator"),
		sigc::mem_fun(*this, &VirtualSurfaceWidget::OnSaveCanvasAs));


	uiManager_ = Gtk::UIManager::create();
	uiManager_->insert_action_group(group);

	std::stringstream buf;
	buf << "<ui>";
       	buf << "  <popup name='PopupMenu'>";
	buf << "     <menuitem action='ContextSave'/>";
	buf << "     <menuitem action='ContextVisualize'/>";
	buf << "     <menuitem action='ContextRefresh'/>";
	buf << "     <menuitem action='ContextCanvasEdit'/>";
	buf << "     <menuitem action='ContextCanvasSaveAs'/>";
	buf << "     <menuitem action='ContextCanvasSave'/>";
       	buf << "  </popup>";
       	buf << "</ui>";
	uiManager_->add_ui_from_string(buf.str());
}

bool VirtualSurfaceWidget::OnClick(GdkEventButton* event)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		((Gtk::Menu*)uiManager_->get_widget("/PopupMenu"))->popup(event->button, event->time);
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
		std::string dir = Glib::get_current_dir();
		dialog.set_current_folder(dir);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
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

