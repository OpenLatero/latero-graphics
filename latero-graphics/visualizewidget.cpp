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

#include "visualizewidget.h"
#include "positiongen.h"
#include <sstream>
#include "units.h"

#define UPDATE_RATE_MS 30

namespace latero {
namespace graphics {

#define mode_abstract "abstract"
#define mode_vib_as_deflection "deflection"
#define mode_vib_as_noise "deflection + vibration"
//#define mode_shadow "shadow"

#define DEFAULT_WIDTH 500

VisualizeWidget::VisualizeWidget(PositionGenPtr gen) :
	Gtk::Window(),
	velMagAdj_(Gtk::Adjustment::create(0,0,10000)),
	velDirAdj_(Gtk::Adjustment::create(0,0,360)),
	intervalCtrl_(10, units::msec),
	frameAdj_(Gtk::Adjustment::create(0,0,10000)),
    nbFramesAdj_(Gtk::Adjustment::create(1,1,10000)),
	widthAdj_(Gtk::Adjustment::create(DEFAULT_WIDTH,50,4000)),
	heightAdj_(Gtk::Adjustment::create(DEFAULT_WIDTH*gen->Dev()->GetSurfaceHeight()/gen->Dev()->GetSurfaceWidth(),50,4000)),
    gen_(gen)
{
	modeList_ = Gtk::StringList::create({});
	modeList_->append(mode_abstract);
	modeList_->append(mode_vib_as_deflection);
	modeList_->append(mode_vib_as_noise);
	modeDropDown_ = Gtk::make_managed<Gtk::DropDown>(modeList_);
	modeDropDown_->set_selected(2); // mode_vib_as_noise

	Gtk::ScrolledWindow *scrolledWindow = Gtk::make_managed<Gtk::ScrolledWindow>();
	scrolledWindow->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
	scrolledWindow->set_child(img_);
	scrolledWindow->set_size_request(5 + widthAdj_->get_value(), 5 + heightAdj_->get_value());

	auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	set_title("Visualization");
	set_modal(true);
	set_child(*vbox);
	vbox->append(*scrolledWindow);
	scrolledWindow->set_vexpand();
	auto playbackWidget = GetPlaybackWidget();
	vbox->append(*playbackWidget);
	playbackWidget->set_margin_bottom(18);
	vbox->append(*hbox);

	auto animWidget = GetAnimWidget();
	animWidget->set_margin_start(12);
	animWidget->set_margin_end(12);
	hbox->append(*animWidget);
	animWidget->set_hexpand();

	CreateMenu();
	auto leftGesture = Gtk::GestureClick::create();
	leftGesture->set_button(GDK_BUTTON_PRIMARY);
	leftGesture->signal_pressed().connect([this](int, double, double){ OnNext(); });
	scrolledWindow->add_controller(leftGesture);

	auto rightGesture = Gtk::GestureClick::create();
	rightGesture->set_button(GDK_BUTTON_SECONDARY);
	rightGesture->signal_pressed().connect([this](int, double x, double y){
		popupMenu_->set_pointing_to(Gdk::Rectangle(x, y, 1, 1));
		popupMenu_->popup();
	});
	scrolledWindow->add_controller(rightGesture);
}

Gtk::Widget *VisualizeWidget::GetAnimWidget()
{
	auto grid = Gtk::make_managed<Gtk::Grid>();

	grid->attach(*Gtk::make_managed<Gtk::Label>("Frames:", Gtk::Align::START),0,0,1,1); // 0,1, 0,1, Gtk::FILL);
	grid->attach(*Gtk::make_managed<Gtk::Label>("Interval:   ", Gtk::Align::START),0,1,1,1); // 0,1, 1,2, Gtk::FILL);
	grid->attach(*Gtk::make_managed<Gtk::Label>("Start:", Gtk::Align::START),0,2,1,1); //0,1, 2,3, Gtk::FILL);

	grid->attach(*Gtk::make_managed<Gtk::SpinButton>(nbFramesAdj_),1,0,1,1); // 1,2, 0,1);
	grid->attach(intervalCtrl_,1,1,1,1); // 1,2, 1,2);
	grid->attach(startTimeCtrl_,1,2,1,1); // 1,2, 2,3);

	grid->attach(*Gtk::make_managed<Gtk::Label>("      Mode:", Gtk::Align::START),2,0,1,1); // 2,3, 0,1, Gtk::FILL);
	grid->attach(*Gtk::make_managed<Gtk::Label>("      Width:", Gtk::Align::START),2,1,1,1); // 2,3, 1,2, Gtk::FILL);
	grid->attach(*Gtk::make_managed<Gtk::Label>("      Velocity:   ", Gtk::Align::START),2,2,1,1); // 2,3, 2,3, Gtk::FILL);

	grid->attach(*modeDropDown_,3,0,4,1); // 3,7, 0,1);
	grid->attach(*Gtk::make_managed<Gtk::SpinButton>(widthAdj_),3,1,1,1); // 3,4, 1,2);
	grid->attach(*Gtk::make_managed<Gtk::SpinButton>(velMagAdj_),3,2,1,1); // 3,4, 2,3);

	grid->attach(*Gtk::make_managed<Gtk::Label>("x"),4,1,1,1); // 4,5, 1,2, Gtk::FILL);
	grid->attach(*Gtk::make_managed<Gtk::Label>(" mm/s ", Gtk::Align::START),4,2,1,1); // 4,5, 2,3, Gtk::FILL);

	grid->attach(*Gtk::make_managed<Gtk::SpinButton>(heightAdj_),5,1,1,1); // 5,6, 1,2);
	grid->attach(*Gtk::make_managed<Gtk::SpinButton>(velDirAdj_),5,2,1,1); // 5,6, 2,3);

	grid->attach(*Gtk::make_managed<Gtk::Label>(" pixels", Gtk::Align::START),6,1,1,1); // 6,7, 1,2, Gtk::FILL);
	grid->attach(*Gtk::make_managed<Gtk::Label>(" degrees", Gtk::Align::START),6,2,1,1); // 6,7, 2,3, Gtk::FILL);

	widthAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &VisualizeWidget::OnWidthChanged));
	heightAdj_->signal_value_changed().connect(sigc::mem_fun(*this, &VisualizeWidget::OnHeightChanged));

	return grid;
}

void VisualizeWidget::OnWidthChanged()
{
	double w = widthAdj_->get_value();
	double h = w*gen_->Dev()->GetSurfaceHeight()/gen_->Dev()->GetSurfaceWidth();
	if (h != heightAdj_->get_value())
		heightAdj_->set_value(h);
}

void VisualizeWidget::OnHeightChanged()
{
	double h = heightAdj_->get_value();
	double w = h*gen_->Dev()->GetSurfaceWidth()/gen_->Dev()->GetSurfaceHeight();
	if (w != widthAdj_->get_value())
		widthAdj_->set_value(w);
}


Gtk::Widget *VisualizeWidget::GetPlaybackWidget()
{
	auto pBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);

	//Gtk::Label *frameLabel = Gtk::make_managed<Gtk::Label>("frame");
	//Gtk::SpinButton *frameCtrl = Gtk::make_managed<Gtk::SpinButton>(frameAdj_);
	//frameCtrl->set_sensitive(false);

	//Gtk::Label *timeLabel = Gtk::make_managed<Gtk::Label>("time (ms)");
	//Gtk::SpinButton *timeCtrl = Gtk::make_managed<Gtk::SpinButton>(timeAdj_);
	//timeCtrl->set_sensitive(false);

	auto prevButton = Gtk::make_managed<Gtk::Button>();
    prevButton->set_icon_name("go-previous-symbolic");
	prevButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnPrevious));

	auto playButton =  Gtk::make_managed<Gtk::Button>();
	playButton->set_icon_name("media-playback-start-symbolic");
	playButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnPlay));

	auto stopButton = Gtk::make_managed<Gtk::Button>();
	stopButton->set_icon_name("media-playback-stop-symbolic");
	stopButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnStop));

	auto nextButton = Gtk::make_managed<Gtk::Button>();
	nextButton->set_icon_name("go-next-symbolic");
	nextButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnNext));

	auto refreshButton = Gtk::make_managed<Gtk::Button>();
	refreshButton->set_icon_name("view-refresh-symbolic");
	refreshButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnApply));

	pBox->append(*prevButton);
	pBox->append(*playButton);
	pBox->append(*stopButton);
	pBox->append(*nextButton);
	nextButton->set_margin_end(12);
	pBox->append(*refreshButton);

	pBox->set_halign(Gtk::Align::CENTER);
	return pBox;
}


VisualizeWidget::~VisualizeWidget()
{
	if (popupMenu_) popupMenu_->unparent();
}


void VisualizeWidget::CreateMenu()
{
	// Create action group and add actions
	auto action_group = Gio::SimpleActionGroup::create();
	action_group->add_action("save",   sigc::mem_fun(*this, &VisualizeWidget::OnSave));
	action_group->add_action("saveas", sigc::mem_fun(*this, &VisualizeWidget::OnSaveAs));
	insert_action_group("viz", action_group);

	// Define the popup menu using Builder XML
	auto builder = Gtk::Builder::create_from_string(R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<interface>
  	<menu id="PopupMenu">
    	<item>
      		<attribute name="label">save as viz.png/gif</attribute>
      		<attribute name="action">viz.save</attribute>
    	</item>
    	<item>
      		<attribute name="label">save as</attribute>
      		<attribute name="action">viz.saveas</attribute>
    	</item>
  	</menu>
	</interface>
	)");

	// Get the menu and create a Gtk::Menu from it
	auto menu_model = std::dynamic_pointer_cast<Gio::MenuModel>(builder->get_object("PopupMenu"));
	popupMenu_ = std::make_unique<Gtk::PopoverMenu>(menu_model);
	popupMenu_->set_parent(*this);
}

void VisualizeWidget::OnSaveAs()
{
	map_.SaveToFile(dynamic_cast<Gtk::Window*>(get_root()));
}

void VisualizeWidget::OnSave()
{
	map_.SaveToFile("viz.png",true);
}

latero::graphics::gtk::Animation VisualizeWidget::GetDeflectionMap(uint w, uint n, double velMag, double velDir)
{
	latero::graphics::gtk::Animation rv;
	boost::posix_time::time_duration timeElapsed = startTimeCtrl_.GetTime();
	boost::posix_time::time_duration delta = intervalCtrl_.GetTime();
	for (unsigned int i=0; i<n; ++i)
	{
		Glib::RefPtr<Gdk::Pixbuf> buf;
		auto modeText = std::string(modeList_->get_string(modeDropDown_->get_selected()));
		if (modeText == mode_vib_as_deflection)
			buf = gen_->GetDeflectionMap(w,timeElapsed,velMag,velDir,RenderMode(true,false,false));
		else if (modeText == mode_vib_as_noise)
			buf = gen_->GetDeflectionMap(w,timeElapsed,velMag,velDir,RenderMode(true,true,false));
		else
			buf = gen_->GetIllustration(w,timeElapsed);
		rv.AppendFrame(buf);

		timeElapsed += delta/1000;
	}
	return rv;
}

void VisualizeWidget::on_realize()
{
	Gtk::Window::on_realize();
	ReloadAnimation();		

	map_.Activate(UPDATE_RATE_MS);
	map_.signal_current_frame_changed.connect(
		sigc::mem_fun(*this, &VisualizeWidget::RefreshImg));
}

void VisualizeWidget::RefreshImg()
{
	frameAdj_->set_value(map_.GetCurrentFrameIndex());
	Glib::RefPtr<Gdk::Pixbuf> buf = map_.GetCurrentFrame();
	img_.set(buf);
}

void VisualizeWidget::ReloadAnimation()
{
	map_ =  GetDeflectionMap(widthAdj_->get_value(),
		nbFramesAdj_->get_value(),
		velMagAdj_->get_value(),
		units::DegreeToRad(velDirAdj_->get_value()));
	RefreshImg();
}

void VisualizeWidget::OnApply()
{
	ReloadAnimation();
}

void VisualizeWidget::OnPlay()
{
	map_.Activate(UPDATE_RATE_MS);
}

void VisualizeWidget::OnStop()
{
	map_.Deactivate();	
}

void VisualizeWidget::OnNext()
{
	map_.IncrementFrame();
	RefreshImg();
}

void VisualizeWidget::OnPrevious()
{
	map_.DecrementFrame();
	RefreshImg();
}

} // namespace graphics
} // namespace latero
