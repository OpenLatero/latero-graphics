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
#include <math.h>
#include "generator.h"
#include "positiongen.h"
#include "visualizewidget.h"
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
		auto drag = Gtk::GestureDrag::create();
		drag->set_button(GDK_BUTTON_PRIMARY);
		drag->signal_drag_begin().connect([this](double x, double y) {
			dev_->SetEmulatedState(GetClickPos(x, y));
		});
		drag->signal_drag_update().connect([this, drag](double offset_x, double offset_y) {
			double start_x, start_y;
			if (drag->get_start_point(start_x, start_y))
				dev_->SetEmulatedState(GetClickPos(start_x + offset_x, start_y + offset_y));
		});
		drag->signal_drag_end().connect([this, drag](double offset_x, double offset_y) {
			double start_x, start_y;
			if (drag->get_start_point(start_x, start_y))
				dev_->SetEmulatedState(GetClickPos(start_x + offset_x, start_y + offset_y));
		});
		add_controller(drag);
	}
    set_draw_func(sigc::mem_fun(*this, &VirtualSurfaceArea::OnDraw));
}

void VirtualSurfaceArea::OnClick(int n_press, double x, double y)
{
	if (!disablePopup_)
	{
		popupMenu_->set_pointing_to(Gdk::Rectangle(x, y, 1, 1));
		popupMenu_->popup();
	}
}

void VirtualSurfaceArea::CreatePopupMenu()
{
	auto gesture = Gtk::GestureClick::create();
	gesture->set_button(GDK_BUTTON_SECONDARY);
	gesture->signal_pressed().connect(sigc::mem_fun(*this, &VirtualSurfaceArea::OnClick));
	add_controller(gesture);

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
	auto menu_model = std::dynamic_pointer_cast<Gio::MenuModel>(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::PopoverMenu>(menu_model);
	popupMenu_->set_parent(*this);
}

void VirtualSurfaceArea::OnSave()
{
	anim_.SaveToFile(dynamic_cast<Gtk::Window*>(get_root()));
}



void VirtualSurfaceArea::SetRounded(bool v)
{
	rounded_ = v;
}

VirtualSurfaceArea::~VirtualSurfaceArea()
{
	if (popupMenu_) popupMenu_->unparent();
	anim_.Deactivate();
}

/*
void VirtualSurfaceArea::on_size_allocate(int width, int height, int baseline)
{
	Gtk::DrawingArea:on_size_allocate(width, height, baseline);
    
	// We can't resize the animation here: quality degrades too quickly.
	// We could keep a second resized copy, but that would require a lot of memory.
}
*/

void VirtualSurfaceArea::OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{
	if (rounded_)
	{
		DrawBorderPath(cr);
    	cr->clip();
    }

    if (!anim_.GetNbFrames())
    {
		//std::cout << "VirtualSurfaceArea::OnDraw(): anim is empty\n";
    	cr->set_source_rgb(1.0, 1.0, 1.0);
    	cr->paint();
        return;
    }

	if (GetWidth() <= 0 || GetHeight() <= 0)
		return;

    Glib::RefPtr<Gdk::Pixbuf> buf = anim_.GetCurrentFrame();
    if (buf)
    {
        // the animation might not have the right size
        buf = buf->scale_simple(GetWidth(),GetHeight(),Gdk::InterpType::NEAREST);
        Gdk::Cairo::set_source_pixbuf(cr, buf, 0, 0);
        cr->paint();
    }

    if (showCursor_)    DrawCursor(cr);
    if (showBorder_)    DrawBorder(cr);
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

	queue_draw();
}

void VirtualSurfaceArea::Clear(guint32 pixel)
{
	if ((GetWidth()<=0)||(GetHeight()<=0))
	{
		std::cout << "VirtualSurfaceArea::Clear() called while width or height is zero. Ignoring.\n";
		return;
	}

	Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create(
			Gdk::Colorspace::RGB, true, 8,
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
    queue_draw();
}


void VirtualSurfaceArea::Set(latero::graphics::gtk::Animation &anim)
{
	anim_ = anim;
	Invalidate();
}

void VirtualSurfaceArea::Set(Glib::RefPtr<Gdk::Pixbuf> buf)
{
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
	auto gesture = Gtk::GestureClick::create();
	gesture->set_button(GDK_BUTTON_SECONDARY);
	gesture->signal_pressed().connect(sigc::mem_fun(*this, &VirtualSurfaceWidget::OnClick));
	add_controller(gesture);

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
	auto menu_model = std::dynamic_pointer_cast<Gio::MenuModel>(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::PopoverMenu>(menu_model);
	popupMenu_->set_parent(*this);
}

void VirtualSurfaceWidget::OnClick(int n_press, double x, double y)
{
	std::cout << "VirtualSurfaceWidget::OnClick\n";
	popupMenu_->set_pointing_to(Gdk::Rectangle(x, y, 1, 1));
	popupMenu_->popup();
}

void VirtualSurfaceWidget::OnSave()
{
	surface_.GetIllustration().SaveToFile(dynamic_cast<Gtk::Window*>(get_root()));
}

void VirtualSurfaceWidget::OnVisualize()
{
	if (peer_)
	{
		PositionGenPtr gen = boost::dynamic_pointer_cast<PositionGen>(peer_);
		if (gen)
		{
			auto dlg = new VisualizeWidget(gen);
			dlg->signal_hide().connect([dlg]{ delete dlg; });
			if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
				dlg->set_transient_for(*win);
			dlg->present();
		}
	}
}

void VirtualSurfaceWidget::OnEdit()
{
	if (peer_)
	{
		auto dlg = new Gtk::Dialog();
		auto widget = Gtk::manage(peer_->CreateWidget(peer_));
		dlg->get_content_area()->append(*widget);
		widget->set_vexpand();
		dlg->signal_hide().connect([dlg]{ delete dlg; });
		if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
			dlg->set_transient_for(*win);
		dlg->present();
	}
}

void VirtualSurfaceWidget::OnSaveCanvasAs()
{
	if (peer_)
	{
		auto dialog = new Gtk::FileChooserDialog("Please select a generator file.", Gtk::FileChooser::Action::SAVE);
		std::string dir = std::filesystem::current_path().string();
		dialog->set_current_folder(Gio::File::create_for_path(dir));
		dialog->add_button("Cancel", Gtk::ResponseType::CANCEL);
		dialog->add_button("Save", Gtk::ResponseType::OK);
		dialog->set_default_response(Gtk::ResponseType::CANCEL);
		dialog->set_current_name("test.gen");
		if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
			dialog->set_transient_for(*win);
		dialog->signal_response().connect([this, dialog](int response_id) {
			if (response_id == Gtk::ResponseType::OK)
			{
				std::string filename = dialog->get_file()->get_path();
				peer_->SaveToFile(filename);
			}
			delete dialog;
		});
		dialog->show();
	}
}

void VirtualSurfaceWidget::OnSaveCanvas()
{
	if (peer_) peer_->SaveToFile("canvas.gen");
}


VirtualSurfaceWidget::~VirtualSurfaceWidget()
{
	if (popupMenu_) popupMenu_->unparent();
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
	{
		// if the background has never been updated or peer has been modified since
		if (bgUpdateTime_.is_not_a_date_time() || (peer_->GetLastModified() > bgUpdateTime_))
			RefreshBackground();
	}
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

void VirtualSurfaceWidget::on_size_allocate(int width, int height, int baseline)
{
	//Gtk::Widget::on_size_allocate(width, height, baseline); // GTKMM4
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

