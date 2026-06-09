#include <filesystem>
#include "tactographview.h"
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

namespace latero::graphics {

CursorLayer::CursorLayer(const latero::Tactograph *dev) : 
	dev_(dev), 
	enable_(false), 
	animate_(true),
	tdAngle_(0),
	tdState_(dev->GetFrameSizeX(), dev->GetFrameSizeY())
 {

 };

void CursorLayer::SetDisplayState(const Point &pos, double angle, const latero::BiasedImg &f)
{
	assert(f.Size() ==  tdState_.Size());

	if (std::isnan(angle)) angle = 0; // for some reason angle is sometimes nan
	tdAngle_ = angle;
	tdPos_ = pos;
	tdState_ = f;
}

void CursorLayer::Draw(const Cairo::RefPtr<Cairo::Context> &mmContext, double dpmm_x)
{
	if (enable_) 
	{
		auto cursorDrawing = GetCursorDrawing(mmContext, dpmm_x);
		mmContext->set_source(cursorDrawing);
		mmContext->paint();
	}
}


Cairo::RefPtr<Cairo::Pattern> CursorLayer::GetCursorDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext, double dpmm_x)
{
	mmContext->push_group();

	double tdWidthPix = dev_->GetWidth() * dpmm_x; 

	/**
	 * @todo
	 * Use the actual position of the piezos used for computations rather
	 * than rotating the frame as done here. This will allow us to
	 * catch any mistakes...
	 */

	if ((tdWidthPix < 15)||!animate_) // TODO: find a good value
	{
		mmContext->translate(tdPos_.x, tdPos_.y);	// shift origin to center of TD
		mmContext->rotate(-tdAngle_);			// line up with TD

		mmContext->set_line_width(0.0);
		mmContext->set_source_rgba(1.0, 0.0, 0.0, 0.5);
		mmContext->rectangle(
			-dev_->GetWidth()/2.0,
			-dev_->GetHeight()/2.0,
			dev_->GetWidth(),
			dev_->GetHeight());
		mmContext->fill();
		mmContext->stroke();
	}
	else
	{
		mmContext->set_source(GetDisplayDrawing(mmContext));
		mmContext->paint();
	}
	return mmContext->pop_group();
}

Cairo::RefPtr<Cairo::Pattern> CursorLayer::GetDisplayDrawing(const Cairo::RefPtr<Cairo::Context> &mmContext)
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
 


void TactographView::OnDraw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{
    if (!anim_.GetNbFrames())
    {
    	cr->set_source_rgb(1.0, 1.0, 1.0);
    	cr->paint();
        return;
    }

	if (width <= 0 || height <= 0)
		return;

    Glib::RefPtr<Gdk::Pixbuf> buf = anim_.GetCurrentFrame();
    if (buf)
    {
        // the animation might not have the right size
        buf = buf->scale_simple(width, height, Gdk::InterpType::NEAREST);
        Gdk::Cairo::set_source_pixbuf(cr, buf, 0, 0);
        cr->paint();
    }

	double dpmm_x = drawingArea_.get_width() / dev_->GetSurfaceWidth();
	double dpmm_y = drawingArea_.get_height() / dev_->GetSurfaceHeight();

	cr->save();
	cr->scale(dpmm_x, dpmm_y); // scale to mm
	cursorLayer_.Draw(cr, dpmm_x);
	cr->restore();
}










void TactographView::ClearBackground(guint32 pixel)
{
	if ((drawingArea_.get_width()<=0)||(drawingArea_.get_height()<=0))
	{
		std::cout << "VirtualSurfaceArea::ClearBackground() called while width or height is zero. Ignoring.\n";
		return;
	}

	Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create(
			Gdk::Colorspace::RGB, true, 8,
			drawingArea_.get_width(), drawingArea_.get_height());
	buf->fill(pixel);
	SetBackground(buf);
}

void TactographView::ShowCursor(bool v)
{
	cursorLayer_.SetEnable(v);
}


void TactographView::SetBackground(latero::graphics::gtk::Animation &anim)
{
	anim_ = anim;
	drawingArea_.queue_draw();
}

void TactographView::SetBackground(Glib::RefPtr<Gdk::Pixbuf> buf)
{
	latero::graphics::gtk::Animation v(buf);
	SetBackground(v);
}

void TactographView::AnimateCursor(bool v)
{
	cursorLayer_.SetAnimate(v);
}
    
Point TactographView::GetClickPos(double x, double y)
{
    return Point(x * dev_->GetSurfaceWidth() / drawingArea_.get_width(), y * dev_->GetSurfaceHeight() / drawingArea_.get_height());
}
    

TactographView::TactographView(const latero::Tactograph *dev, GeneratorPtr gen, bool refreshBackground) :
 	Gtk::AspectFrame(0.5, 0.5, dev->GetSurfaceWidth()/dev->GetSurfaceHeight(), false),
	dev_(dev),
	cursorLayer_(dev),
	peer_(gen)
{
	ClearBackground(0xffffffff);

	// TODO: The following was removed to temporarily disable animation. This saves a lot
	// of processor time and makes the user interface much smoother.

	//anim_.Activate(UPDATE_RATE_MS);
	//anim_.signal_current_frame_changed.connect(
	//	sigc::mem_fun(*this, &VirtualSurfaceArea::Invalidate)); // Invalidate -> drawingArea_.queue_draw();

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
		drawingArea_.add_controller(drag);
	}
    drawingArea_.set_draw_func(sigc::mem_fun(*this, &TactographView::OnDraw));
	set_child(drawingArea_);
	drawingArea_.set_expand();	

	drawingArea_.signal_resize().connect([this](int, int){ RefreshBackground(); });

	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &TactographView::RefreshCursor),
		(uint)33, // ms
		Glib::PRIORITY_DEFAULT_IDLE);

	if (refreshBackground)
	{
		Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &TactographView::OnCheckPeer),
			(uint)333, // ms
			Glib::PRIORITY_DEFAULT_IDLE);
	}

	CreatePopupMenu();
}

void TactographView::CreatePopupMenu()
{
	auto gesture = Gtk::GestureClick::create();
	gesture->set_button(GDK_BUTTON_SECONDARY);
	gesture->signal_pressed().connect(sigc::mem_fun(*this, &TactographView::OnClick));
	add_controller(gesture);

	// Create action group and add actions
	auto action_group = Gio::SimpleActionGroup::create();
	action_group->add_action("save",     sigc::mem_fun(*this, &TactographView::OnSave));
	action_group->add_action("visualize",sigc::mem_fun(*this, &TactographView::OnVisualize));
	action_group->add_action("refresh",  sigc::mem_fun(*this, &TactographView::RefreshBackground));
	action_group->add_action("edit",     sigc::mem_fun(*this, &TactographView::OnEdit));
	action_group->add_action("savecanvas",   sigc::mem_fun(*this, &TactographView::OnSaveCanvas));
	action_group->add_action("savecanvases", sigc::mem_fun(*this, &TactographView::OnSaveCanvasAs));
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

void TactographView::OnClick(int n_press, double x, double y)
{
	popupMenu_->set_pointing_to(Gdk::Rectangle(x, y, 1, 1));
	popupMenu_->popup();
}

void TactographView::OnSave()
{
	anim_.SaveToFile(dynamic_cast<Gtk::Window*>(get_root()));
}

void TactographView::OnVisualize()
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

void TactographView::OnEdit()
{
	if (peer_)
	{
		auto dlg = new Gtk::Window();
		auto widget = peer_->CreateWidget(peer_);
		dlg->set_child(*widget);
		dlg->set_modal(true);
		dlg->signal_hide().connect([dlg]{ delete dlg; });
		if (auto* win = dynamic_cast<Gtk::Window*>(get_root()))
			dlg->set_transient_for(*win);
		dlg->present();
	}
}

void TactographView::OnSaveCanvasAs()
{
	if (peer_)
	{
		auto dialog = Gtk::FileDialog::create();
		dialog->set_title("Please select a generator file.");
		dialog->set_initial_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
		dialog->set_initial_name("test.gen");

		auto* win = dynamic_cast<Gtk::Window*>(get_root());
		dialog->save(*win, [this, dialog](Glib::RefPtr<Gio::AsyncResult>& result) {
			try {
				auto file = dialog->save_finish(result);
				peer_->SaveToFile(file->get_path());
			} catch (const Gtk::DialogError&) {}
		});
	}
}

void TactographView::OnSaveCanvas()
{
	if (peer_) peer_->SaveToFile("canvas.gen");
}


TactographView::~TactographView()
{
	anim_.Deactivate();
	if (popupMenu_) popupMenu_->unparent();
}

bool TactographView::RefreshCursor()
{
	if (peer_)
	{
		Point pos = peer_->GetDisplayCenter();
		double angle = peer_->GetDisplayOrientation();
		latero::BiasedImg frame = peer_->GetLatestFrame();
		cursorLayer_.SetDisplayState(pos, angle, frame);
		drawingArea_.queue_draw();
	}
	return true;
}


bool TactographView::OnCheckPeer()
{
	if (peer_)
	{
		// if the background has never been updated or peer has been modified since
		if (bgUpdateTime_.is_not_a_date_time() || (peer_->GetLastModified() > bgUpdateTime_))
			RefreshBackground();
	}
	return true;
}


void TactographView::RefreshBackground()
{
	bgUpdateTime_ = boost::posix_time::microsec_clock::universal_time();
	if (peer_)
	{
		auto anim = peer_->GetIllustration(drawingArea_.get_width(), boost::posix_time::seconds(0));
		SetBackground(anim);
	}
}


void TactographView::SetGenerator(GeneratorPtr gen)
{
	peer_ = gen;
	RefreshBackground();
}

} // namespace

