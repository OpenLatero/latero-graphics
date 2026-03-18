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
#include "../grating.h"
#include "dotpatternwidget.h"

namespace latero {
namespace graphics { 

class StrokeIntensityCtrl : public Gtk::Box
{
public:
	StrokeIntensityCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetIntensity()*100,0,100)), peer_(peer)
	{
		append(*Gtk::manage(new gtk::HNumWidget("stroke intensity", adj_, 1, "%")));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &StrokeIntensityCtrl::OnChanged));
	}
	virtual ~StrokeIntensityCtrl() {};
protected:
	void OnChanged() { peer_->SetIntensity(adj_->get_value()/100); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	StrokePtr peer_;
};


class StrokeWidthCtrl : public Gtk::Box
{
public:
	StrokeWidthCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetWidth(),0,50.0)), peer_(peer)
	{
		append(*Gtk::manage(new gtk::HNumWidget("stroke width", adj_, 1, "mm")));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &StrokeWidthCtrl::OnChanged));
	}
	virtual ~StrokeWidthCtrl() {};
protected:
	void OnChanged() { peer_->SetWidth(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;	
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
		auto miscBox = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
		auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
		auto amplitudeWidget = Gtk::manage(new GratingAmplitudeWidget(peer->GetTexture()));
		auto invertCtrl = Gtk::manage(new GratingInvertCtrl(peer->GetTexture()));
		auto pitchWidget = Gtk::manage(new GratingPitchWidget(peer->GetTexture()));

		amplitudeWidget->set_hexpand();
		pitchWidget->set_hexpand();
		miscBox->set_hexpand();

		miscBox->append(*amplitudeWidget);
		miscBox->append(*invertCtrl);
		GetBox().append(*box);
		box->append(*pitchWidget);
		box->append(*miscBox);
	
		GetCheck().signal_toggled().connect(sigc::mem_fun(*this, &StrokeTextureWidget::OnEnable));
	};
	virtual ~StrokeTextureWidget() {};
protected:
	void OnEnable() { peer_->GetTexture()->SetEnable(GetCheck().get_active()); };
	StrokePtr peer_;
};

class StrokeVibrationCtrl : public Gtk::Box
{
public:
	StrokeVibrationCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL)
	{
		gtk::CheckFrame *vib = Gtk::manage(new OscillatorWidget(peer->GetOscillator(), true));
		append(*vib);
	}
	virtual ~StrokeVibrationCtrl() {}
};




StrokeProfileWidget::StrokeProfileWidget(StrokePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL)
{
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
	auto strokeWidthCtrl = Gtk::manage(new StrokeWidthCtrl(peer));
	auto strokeIntensityCtrl = Gtk::manage(new StrokeIntensityCtrl(peer));
	auto ridgeEdgeWidthScale = Gtk::manage(new RidgeEdgeWidthScale(peer->GetProfile()));
	auto ridgeTextureCtrl = Gtk::manage(new RidgeTextureCtrl(peer->GetProfile()));
	auto ridgeGraph = Gtk::manage(new RidgeGraph(peer->GetProfile(),300));

	strokeWidthCtrl->set_vexpand();
	strokeIntensityCtrl->set_vexpand();
	ridgeEdgeWidthScale->set_vexpand();
	ridgeTextureCtrl->set_vexpand();
	strokeWidthCtrl->set_hexpand();

	box->append(*strokeWidthCtrl);
	box->append(*strokeIntensityCtrl);
	box->append(*ridgeEdgeWidthScale);
	box->append(*ridgeTextureCtrl);

	append(*box);
	append(*ridgeGraph);
};


StrokeFillWidget::StrokeFillWidget(StrokePtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL)
{
	Gtk::Frame *frame = Gtk::manage(new Gtk::Frame("general"));
	frame->set_child(*Gtk::manage(new StrokeGutterCheck(peer)));

	auto vibrationCtrl = Gtk::manage(new StrokeVibrationCtrl(peer));
	auto textureWidget = Gtk::manage(new StrokeTextureWidget(peer));

	frame->set_vexpand();
	vibrationCtrl->set_vexpand();
	textureWidget->set_vexpand();

	append(*frame);
	append(*vibrationCtrl);
	append(*textureWidget);

	frame->set_vexpand();
	vibrationCtrl->set_vexpand();
	textureWidget->set_vexpand();
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


StrokeDottedWidget::StrokeDottedWidget(StrokePtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL)
{
	append(*Gtk::manage(new StrokeDotEnableCheck(peer)));
	DotPatternPtr p = peer->GetDots();
	if (p) append(*Gtk::manage(new DotPatternMinSpacingCtrl(p)));
};

} // namespace graphics
} // namespace latero

