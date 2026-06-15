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
	tdPainter_.Paint(cr, tdState_, tdPos_.x, tdPos_.y, tdAngle_);
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
	tdPainter_.Enable(v);
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
	tdPainter_.ForceSimple(!v);
}
    
Point TactographView::GetClickPos(double x, double y)
{
    return Point(x * dev_->GetSurfaceWidth() / drawingArea_.get_width(), y * dev_->GetSurfaceHeight() / drawingArea_.get_height());
}
    

TactographView::TactographView(const latero::Tactograph *dev, GeneratorPtr gen, bool refreshBackground) :
 	Gtk::AspectFrame(0.5, 0.5, dev->GetSurfaceWidth()/dev->GetSurfaceHeight(), false),
	dev_(dev),
	peer_(gen),
	tdAngle_(0),
	tdState_(dev->GetFrameSizeX(), dev->GetFrameSizeY()),
	tdPainter_(dev)
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
		SetDisplayState(pos, angle, frame);
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

void TactographView::SetDisplayState(const Point &pos, double angle, const latero::BiasedImg &f)
{
	assert(f.Size() ==  tdState_.Size());

	if (std::isnan(angle)) angle = 0; // for some reason angle is sometimes nan
	tdAngle_ = angle;
	tdPos_ = pos;
	tdState_ = f;
}


} // namespace

