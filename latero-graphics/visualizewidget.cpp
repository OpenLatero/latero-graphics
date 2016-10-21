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
#include <gtkmm.h>
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
	Gtk::Dialog("Visualization"),
	velMagAdj_(0,0,10000),
	velDirAdj_(0,0,360),
	intervalCtrl_(10, units::msec),
	frameAdj_(0,0,10000),
    nbFramesAdj_(1,1,10000),
	widthAdj_(DEFAULT_WIDTH,50,4000),
	heightAdj_(DEFAULT_WIDTH*gen->Dev()->GetSurfaceHeight()/gen->Dev()->GetSurfaceWidth(),50,4000),
    gen_(gen)
{
	modeCombo_.append_text(mode_abstract);
	modeCombo_.append_text(mode_vib_as_deflection);
	modeCombo_.append_text(mode_vib_as_noise);
	modeCombo_.set_active_text(mode_vib_as_noise);

	Gtk::ScrolledWindow *scrolledWindow = manage(new Gtk::ScrolledWindow);
	scrolledWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrolledWindow->add(img_);
	scrolledWindow->set_size_request(5 + widthAdj_.get_value(), 5 + heightAdj_.get_value());

	Gtk::HBox *hbox = manage(new Gtk::HBox);
	get_vbox()->pack_start(*scrolledWindow);
	get_vbox()->pack_start(*GetPlaybackWidget(), Gtk::PACK_SHRINK);
	get_vbox()->pack_start(*manage(new gtk::VSpacer(18)), Gtk::PACK_SHRINK);
	get_vbox()->pack_start(*hbox, Gtk::PACK_SHRINK);

	hbox->pack_start(*manage(new gtk::HSpacer(12)), Gtk::PACK_SHRINK);
	hbox->pack_start(*GetAnimWidget());
	hbox->pack_start(*manage(new gtk::HSpacer(12)), Gtk::PACK_SHRINK);

	CreateMenu();
	scrolledWindow->set_events(Gdk::BUTTON_PRESS_MASK);
	scrolledWindow->signal_button_press_event().connect(sigc::mem_fun(*this, &VisualizeWidget::OnClick));

	show_all_children();
}

Gtk::Widget *VisualizeWidget::GetAnimWidget()
{
	Gtk::Table *table = manage(new Gtk::Table(6,3));

	table->attach(*manage(new Gtk::Label("Frames:", Gtk::ALIGN_LEFT)), 	0,1, 0,1, Gtk::FILL);
	table->attach(*manage(new Gtk::Label("Interval:", Gtk::ALIGN_LEFT)), 	0,1, 1,2, Gtk::FILL);
	table->attach(*manage(new Gtk::Label("Start:", Gtk::ALIGN_LEFT)), 	0,1, 2,3, Gtk::FILL);
	table->set_col_spacing(0,12);

	table->attach(*manage(new Gtk::SpinButton(nbFramesAdj_)),		1,2, 0,1);
	table->attach(intervalCtrl_,				 		1,2, 1,2);
	table->attach(startTimeCtrl_,						1,2, 2,3);
	table->set_col_spacing(1,30);

	table->attach(*manage(new Gtk::Label("Mode:", Gtk::ALIGN_LEFT)), 	2,3, 0,1, Gtk::FILL);
	table->attach(*manage(new Gtk::Label("Width:", Gtk::ALIGN_LEFT)), 	2,3, 1,2, Gtk::FILL);
	table->attach(*manage(new Gtk::Label("Velocity:", Gtk::ALIGN_LEFT)), 	2,3, 2,3, Gtk::FILL);
	table->set_col_spacing(2,12);

	table->attach(modeCombo_,				 		3,7, 0,1);
	table->attach(*manage(new Gtk::SpinButton(widthAdj_)),			3,4, 1,2);
	table->attach(*manage(new Gtk::SpinButton(velMagAdj_)), 		3,4, 2,3);
	table->set_col_spacing(3,6);

	table->attach(*manage(new Gtk::Label("x")), 				4,5, 1,2, Gtk::FILL);
	table->attach(*manage(new Gtk::Label("mm/s", Gtk::ALIGN_LEFT)), 	4,5, 2,3, Gtk::FILL);
	table->set_col_spacing(4,12);

	table->attach(*manage(new Gtk::SpinButton(heightAdj_)),			5,6, 1,2);
	table->attach(*manage(new Gtk::SpinButton(velDirAdj_)), 		5,6, 2,3);
	table->set_col_spacing(5,6);

	table->attach(*manage(new Gtk::Label("pixels", Gtk::ALIGN_LEFT)), 	6,7, 1,2, Gtk::FILL);
	table->attach(*manage(new Gtk::Label("degrees", Gtk::ALIGN_LEFT)), 	6,7, 2,3, Gtk::FILL);

	widthAdj_.signal_value_changed().connect(sigc::mem_fun(*this, &VisualizeWidget::OnWidthChanged));
	heightAdj_.signal_value_changed().connect(sigc::mem_fun(*this, &VisualizeWidget::OnHeightChanged));

	return table;
}

void VisualizeWidget::OnWidthChanged()
{
	double w = widthAdj_.get_value();
	double h = w*gen_->Dev()->GetSurfaceHeight()/gen_->Dev()->GetSurfaceWidth();
	if (h != heightAdj_.get_value())
		heightAdj_.set_value(h);
}

void VisualizeWidget::OnHeightChanged()
{
	double h = heightAdj_.get_value();
	double w = h*gen_->Dev()->GetSurfaceWidth()/gen_->Dev()->GetSurfaceHeight();
	if (w != widthAdj_.get_value())
		widthAdj_.set_value(w);
}


Gtk::Widget *VisualizeWidget::GetPlaybackWidget()
{
	Gtk::Alignment *pAlignBox = manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0, 0));
	Gtk::HBox *pBox = manage(new Gtk::HBox);

	//Gtk::Label *frameLabel = manage(new Gtk::Label("frame"));
	//Gtk::SpinButton *frameCtrl = manage(new Gtk::SpinButton(frameAdj_));
	//frameCtrl->set_sensitive(false);

	//Gtk::Label *timeLabel = manage(new Gtk::Label("time (ms)"));
	//Gtk::SpinButton *timeCtrl = manage(new Gtk::SpinButton(timeAdj_));
	//timeCtrl->set_sensitive(false);


	Gtk::Button *prevButton =  manage(new Gtk::Button);
        prevButton->add(*manage(new Gtk::Image(Gtk::Stock::MEDIA_PREVIOUS, Gtk::ICON_SIZE_BUTTON))); 
	prevButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnPrevious));
	
	Gtk::Button *playButton =  manage(new Gtk::Button);
	playButton->add(*manage(new Gtk::Image(Gtk::Stock::MEDIA_PLAY, Gtk::ICON_SIZE_BUTTON))); 
	playButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnPlay));


	Gtk::Button *stopButton =  manage(new Gtk::Button);
	stopButton->add(*manage(new Gtk::Image(Gtk::Stock::MEDIA_STOP, Gtk::ICON_SIZE_BUTTON))); 
	stopButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnStop));

	Gtk::Button *nextButton =  manage(new Gtk::Button);
	nextButton->add(*manage(new Gtk::Image(Gtk::Stock::MEDIA_NEXT, Gtk::ICON_SIZE_BUTTON))); 
	nextButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnNext));

	Gtk::Button *refreshButton =  manage(new Gtk::Button);
	refreshButton->add(*manage(new Gtk::Image(Gtk::Stock::REFRESH, Gtk::ICON_SIZE_BUTTON))); 
	refreshButton->signal_clicked().connect(sigc::mem_fun(*this, &VisualizeWidget::OnApply));

	pBox->pack_start(*prevButton, Gtk::PACK_SHRINK);
	pBox->pack_start(*playButton, Gtk::PACK_SHRINK);
	pBox->pack_start(*stopButton, Gtk::PACK_SHRINK);
	pBox->pack_start(*nextButton, Gtk::PACK_SHRINK);
	pBox->pack_start(*manage(new gtk::HSpacer(12)), Gtk::PACK_SHRINK);
	pBox->pack_start(*refreshButton, Gtk::PACK_SHRINK);

	pAlignBox->add(*pBox);
	return pAlignBox;
}


VisualizeWidget::~VisualizeWidget()
{
}

bool VisualizeWidget::OnClick(GdkEventButton* event)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1))
	{
		OnNext();
        return true;
	}
	else if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		((Gtk::Menu*)uiManager_->get_widget("/PopupMenu"))->popup(event->button, event->time);
		return true;
	}
	else
		return false;
}


void VisualizeWidget::CreateMenu()
{
	Glib::RefPtr<Gtk::ActionGroup> group = Gtk::ActionGroup::create();
	group->add(Gtk::Action::create("ContextSave", "save as viz.png/gif", "save image"),
		sigc::mem_fun(*this, &VisualizeWidget::OnSave));
	group->add(Gtk::Action::create("ContextSaveAs", "save as", "save as image"),
		sigc::mem_fun(*this, &VisualizeWidget::OnSaveAs));

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

void VisualizeWidget::OnSaveAs()
{
	map_.SaveToFile();
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
		if (modeCombo_.get_active_text() == mode_vib_as_deflection)
			buf = gen_->GetDeflectionMap(w,timeElapsed,velMag,velDir,RenderMode(true,false,false));
		else if (modeCombo_.get_active_text() == mode_vib_as_noise)
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
	Gtk::Dialog::on_realize();
	ReloadAnimation();		

	map_.Activate(UPDATE_RATE_MS);
	map_.signal_current_frame_changed.connect(
		sigc::mem_fun(*this, &VisualizeWidget::RefreshImg));
}

void VisualizeWidget::RefreshImg()
{
	frameAdj_.set_value(map_.GetCurrentFrameIndex());
	Glib::RefPtr<Gdk::Pixbuf> buf = map_.GetCurrentFrame();
	img_.set(buf);
	show_all_children();
}

void VisualizeWidget::ReloadAnimation()
{
	map_ =  GetDeflectionMap(widthAdj_.get_value(),
		nbFramesAdj_.get_value(),
		velMagAdj_.get_value(),
		units::DegreeToRad(velDirAdj_.get_value()));
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
