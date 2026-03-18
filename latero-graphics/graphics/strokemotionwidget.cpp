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

#include "strokemotionwidget.h"
#include "stroke.h"
#include "../gtk/numwidget.h"
#include "../gratingwidget.h"
#include "../ridgewidget.h"
#include "texture/vibrotexturewidget.h"
#include "../oscillatorwidget.h"
#include "../grating.h"

namespace latero {
namespace graphics { 


class StrokeMotionCueCombo : public Gtk::ComboBoxText
{
public:
	StrokeMotionCueCombo(StrokePtr peer) : peer_(peer)
	{
		Stroke::MotionCueSet ops = peer->GetMotionCues();
		for (unsigned int i=0; i<ops.size(); ++i)
			append(ops[i].label);
		set_active_text(peer->GetMotionCue().label);
		signal_changed().connect(sigc::mem_fun(*this, &StrokeMotionCueCombo::OnChange));
	};
	virtual ~StrokeMotionCueCombo() {};
private:
	void OnChange() { peer_->SetMotionCue(get_active_text()); };
	StrokePtr peer_;
};


class StrokeMinMotionWidthCtrl : public Gtk::Box
{
public:
	StrokeMinMotionWidthCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetMinMotionWidth(),0,20)), peer_(peer)
	{
		add(*Gtk::manage(new gtk::HNumWidget("min width", adj_, 1, units::mm)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &StrokeMinMotionWidthCtrl::OnChanged));
	}
	virtual ~StrokeMinMotionWidthCtrl() {};
protected:
	void OnChanged() { peer_->SetMinMotionWidth(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	StrokePtr peer_;
};

class StrokeMotionVelCtrl : public Gtk::Box
{
public:
	StrokeMotionVelCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetMotionVelocity(),-100,100)), peer_(peer)
	{
		add(*Gtk::manage(new gtk::HNumWidget("velocity", adj_, 1, units::mm_per_sec)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &StrokeMotionVelCtrl::OnChanged));
	}
	virtual ~StrokeMotionVelCtrl() {};
protected:
	void OnChanged() { peer_->SetMotionVelocity(adj_->get_value()); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	StrokePtr peer_;
};

class StrokeSuperposedMotionRatioCtrl : public Gtk::Box
{
public:
	StrokeSuperposedMotionRatioCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetSuperposedMotionRatio()*100,0,100)), peer_(peer)
	{
		add(*Gtk::manage(new gtk::HNumWidget("ratio", adj_, 0, units::percent)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &StrokeSuperposedMotionRatioCtrl::OnChanged));
	}
	virtual ~StrokeSuperposedMotionRatioCtrl() {};
protected:
	void OnChanged() { peer_->SetSuperposedMotionRatio(adj_->get_value()/100); };
    Glib::RefPtr<Gtk::Adjustment> adj_;
	StrokePtr peer_;
};


class StrokeBlendMotionValueCtrl : public Gtk::Box
{
public:
	StrokeBlendMotionValueCtrl(StrokePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetBlendMotionValue()*100,0,100)), peer_(peer)
	{
		add(*Gtk::manage(new gtk::HNumWidget("blend value", adj_, 0, units::percent)));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &StrokeBlendMotionValueCtrl::OnChanged));
	}
	virtual ~StrokeBlendMotionValueCtrl() {};
protected:
	void OnChanged() { peer_->SetBlendMotionValue(adj_->get_value()/100); };
    Glib::RefPtr<Gtk::Adjustment> adj_;	
	StrokePtr peer_;
};


StrokeMotionWidget::StrokeMotionWidget(StrokePtr peer) : Gtk::Box(Gtk::Orientation::VERTICAL), peer_(peer)
{
	holder_.set_shadow_type(Gtk::SHADOW_NONE);

	velWidget_ = Gtk::manage(new StrokeMotionVelCtrl(peer));
	StrokeMotionCueCombo *wCue = Gtk::manage(new StrokeMotionCueCombo(peer));
	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));

	velWidget_->set_hexpand();

	box->append(*wCue);
	box->append(*velWidget_);

	append(*box);
	append(holder_);
	
	Rebuild();

	wCue->signal_changed().connect(sigc::mem_fun(*this, &StrokeMotionWidget::OnCueChanged));
};

void StrokeMotionWidget::OnCueChanged()
{
	Rebuild();
}

void StrokeMotionWidget::Rebuild()
{
	Gtk::Widget *wp = holder_.get_child();
	holder_.remove();
	delete wp;

	Stroke::MotionCue cue = peer_->GetMotionCue();

	velWidget_->set_sensitive(!(cue == Stroke::motion_cue_none));

	auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL));
	holder_.add(*box);
	if (cue == Stroke::motion_cue_none)
	{
	}
	else if (cue == Stroke::motion_cue_inherent)
	{
	}
	else if (cue == Stroke::motion_cue_superposed)
	{
		auto hbox = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
		auto strokeMinMotionWidthCtrl = Gtk::manage(new StrokeMinMotionWidthCtrl(peer_));
		auto strokeSuperposedMotionRatioCtrl = Gtk::manage(new StrokeSuperposedMotionRatioCtrl(peer_));
		auto gratingPitchWidget = Gtk::manage(new GratingPitchWidget(peer_->GetMotionTexture()));
		auto oscillatorWidget = Gtk::manage(new OscillatorWidget(peer_->GetMotionOscillator()));

		strokeMinMotionWidthCtrl->set_hexpand();
		strokeSuperposedMotionRatioCtrl->set_hexpand();
		hbox->set_vexpand();

		hbox->append(*strokeMinMotionWidthCtrl);
		hbox->append(*strokeSuperposedMotionRatioCtrl);
		box->append(*hbox);
		box->append(*gratingPitchWidget);
		box->append(*oscillatorWidget);
	}
	else if (cue == Stroke::motion_cue_suppressed)
	{
		box->append(*Gtk::manage(new StrokeMinMotionWidthCtrl(peer_)));
		box->append(*Gtk::manage(new GratingPitchWidget(peer_->GetMotionTexture())));
		box->append(*Gtk::manage(new OscillatorWidget(peer_->GetMotionOscillator())));
	}
	else if (cue == Stroke::motion_cue_vib)
	{
		box->append(*Gtk::manage(new StrokeMinMotionWidthCtrl(peer_)));
		box->append(*Gtk::manage(new GratingPitchWidget(peer_->GetMotionTexture())));
		Gtk::Frame *frame = Gtk::manage(new Gtk::Frame("frequency"));
		frame->set_child(*Gtk::manage(new OscillatorFreqCtrl(peer_->GetMotionOscillator())));
		box->append(*frame);
	}
	else if (cue == Stroke::motion_cue_blend)
	{
		box->append(*Gtk::manage(new StrokeMinMotionWidthCtrl(peer_)));
		box->append(*Gtk::manage(new GratingPitchWidget(peer_->GetMotionTexture())));
		box->append(*Gtk::manage(new StrokeBlendMotionValueCtrl(peer_)));
	}
}

} // namespace graphics
} // namespace latero

