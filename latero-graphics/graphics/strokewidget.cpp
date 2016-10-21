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

#include "strokewidget.h"
#include "stroke.h"
#include "../gtk/numwidget.h"
#include "../gratingwidget.h"
#include "../ridgewidget.h"
#include "texture/vibrotexturewidget.h"
#include "../oscillatorwidget.h"
#include <gtkmm.h>
#include "../grating.h"
#include "dotpatternwidget.h"

namespace latero {
namespace graphics { 

class StrokeIntensityCtrl : public Gtk::VBox
{
public:
	StrokeIntensityCtrl(StrokePtr peer) : 
		adj_(peer->GetIntensity()*100,0,100), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("stroke intensity", adj_, 1, "%")));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &StrokeIntensityCtrl::OnChanged));
	}
	virtual ~StrokeIntensityCtrl() {};
protected:
	void OnChanged() { peer_->SetIntensity(adj_.get_value()/100); };
	Gtk::Adjustment adj_;	
	StrokePtr peer_;
};


class StrokeWidthCtrl : public Gtk::VBox
{
public:
	StrokeWidthCtrl(StrokePtr peer) : 
		adj_(peer->GetWidth(),0,50.0), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("stroke width", adj_, 1, "mm")));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &StrokeWidthCtrl::OnChanged));
	}
	virtual ~StrokeWidthCtrl() {};
protected:
	void OnChanged() { peer_->SetWidth(adj_.get_value()); };
	Gtk::Adjustment adj_;	
	StrokePtr peer_;
};



class StrokeGutterCheck : public Gtk::CheckButton
{
public:
	StrokeGutterCheck(StrokePtr peer) :
		Gtk::CheckButton("gutter"), peer_(peer)
	{
		set_active(peer_->GetGutterEnable());
		signal_toggled().connect(sigc::mem_fun(*this, &StrokeGutterCheck::OnChanged));
	}
	virtual ~StrokeGutterCheck() {}
protected:
	void OnChanged() { peer_->SetGutterEnable(get_active()); }
	StrokePtr peer_;
};



class StrokeTextureWidget : public gtk::CheckFrame
{
public:
	StrokeTextureWidget(StrokePtr peer) : gtk::CheckFrame(peer->GetTexture()->GetEnable(), "grating"), peer_(peer)
	{
		Gtk::HBox *miscBox = manage(new Gtk::HBox);

		miscBox->pack_start(*manage(new GratingAmplitudeWidget(peer->GetTexture())));
		miscBox->pack_start(*manage(new GratingInvertCtrl(peer->GetTexture())),Gtk::PACK_SHRINK);

		Gtk::VBox *box = manage(new Gtk::VBox);
		GetBox().add(*box);

		box->pack_start(*manage(new GratingPitchWidget(peer->GetTexture())));
		box->pack_start(*miscBox);
	
		GetCheck().signal_clicked().connect(sigc::mem_fun(*this, &StrokeTextureWidget::OnEnable));
	};
	virtual ~StrokeTextureWidget() {};
protected:
	void OnEnable() { peer_->GetTexture()->SetEnable(GetCheck().get_active()); };
	StrokePtr peer_;
};

class StrokeVibrationCtrl : public Gtk::HBox
{
public:
	StrokeVibrationCtrl(StrokePtr peer)
	{
		gtk::CheckFrame *vib = manage(new OscillatorWidget(peer->GetOscillator(), true));
		add(*vib);
	}
	virtual ~StrokeVibrationCtrl() {}
};




StrokeProfileWidget::StrokeProfileWidget(StrokePtr peer)
{
	Gtk::VBox *box = manage(new Gtk::VBox);
	box->pack_start(*manage(new StrokeWidthCtrl(peer)));
	box->pack_start(*manage(new StrokeIntensityCtrl(peer)));
	box->pack_start(*manage(new RidgeEdgeWidthScale(peer->GetProfile())));
	box->pack_start(*manage(new RidgeTextureCtrl(peer->GetProfile())));

	pack_start(*box);
	pack_start(*manage(new RidgeGraph(peer->GetProfile(),300)), Gtk::PACK_SHRINK);
};


StrokeFillWidget::StrokeFillWidget(StrokePtr peer)
{
	Gtk::Frame *frame = manage(new Gtk::Frame("general"));
	frame->add(*manage(new StrokeGutterCheck(peer)));

	pack_start(*frame);
	pack_start(*manage(new StrokeVibrationCtrl(peer)));
	pack_start(*manage(new StrokeTextureWidget(peer)));
};





class StrokeDotEnableCheck : public Gtk::CheckButton
{
public:
	StrokeDotEnableCheck(StrokePtr peer) :
		Gtk::CheckButton("enable"), peer_(peer)
	{
		set_active(peer_->GetDotEnable());
		signal_toggled().connect(sigc::mem_fun(*this, &StrokeDotEnableCheck::OnChanged));
	}
	virtual ~StrokeDotEnableCheck() {}
protected:
	void OnChanged() { peer_->SetDotEnable(get_active()); }
	StrokePtr peer_;
};


StrokeDottedWidget::StrokeDottedWidget(StrokePtr peer)
{
	pack_start(*manage(new StrokeDotEnableCheck(peer)), Gtk::PACK_SHRINK);
	DotPatternPtr p = peer->GetDots();
	if (p) pack_start(*manage(new DotPatternMinSpacingCtrl(p)), Gtk::PACK_SHRINK);
};

} // namespace graphics
} // namespace latero

