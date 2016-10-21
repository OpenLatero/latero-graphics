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
			append_text(ops[i].label);
		set_active_text(peer->GetMotionCue().label);
		signal_changed().connect(sigc::mem_fun(*this, &StrokeMotionCueCombo::OnChange));
	};
	virtual ~StrokeMotionCueCombo() {};
private:
	void OnChange() { peer_->SetMotionCue(get_active_text()); };
	StrokePtr peer_;
};


class StrokeMinMotionWidthCtrl : public Gtk::VBox
{
public:
	StrokeMinMotionWidthCtrl(StrokePtr peer) : 
		adj_(peer->GetMinMotionWidth(),0,20), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("min width", adj_, 1, units::mm)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &StrokeMinMotionWidthCtrl::OnChanged));
	}
	virtual ~StrokeMinMotionWidthCtrl() {};
protected:
	void OnChanged() { peer_->SetMinMotionWidth(adj_.get_value()); };
	Gtk::Adjustment adj_;	
	StrokePtr peer_;
};

class StrokeMotionVelCtrl : public Gtk::VBox
{
public:
	StrokeMotionVelCtrl(StrokePtr peer) : 
		adj_(peer->GetMotionVelocity(),-100,100), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("velocity", adj_, 1, units::mm_per_sec)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &StrokeMotionVelCtrl::OnChanged));
	}
	virtual ~StrokeMotionVelCtrl() {};
protected:
	void OnChanged() { peer_->SetMotionVelocity(adj_.get_value()); };
	Gtk::Adjustment adj_;	
	StrokePtr peer_;
};

class StrokeSuperposedMotionRatioCtrl : public Gtk::VBox
{
public:
	StrokeSuperposedMotionRatioCtrl(StrokePtr peer) : 
		adj_(peer->GetSuperposedMotionRatio()*100,0,100), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("ratio", adj_, 0, units::percent)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &StrokeSuperposedMotionRatioCtrl::OnChanged));
	}
	virtual ~StrokeSuperposedMotionRatioCtrl() {};
protected:
	void OnChanged() { peer_->SetSuperposedMotionRatio(adj_.get_value()/100); };
	Gtk::Adjustment adj_;	
	StrokePtr peer_;
};


class StrokeBlendMotionValueCtrl : public Gtk::VBox
{
public:
	StrokeBlendMotionValueCtrl(StrokePtr peer) : 
		adj_(peer->GetBlendMotionValue()*100,0,100), peer_(peer)
	{
		add(*manage(new gtk::HNumWidget("blend value", adj_, 0, units::percent)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &StrokeBlendMotionValueCtrl::OnChanged));
	}
	virtual ~StrokeBlendMotionValueCtrl() {};
protected:
	void OnChanged() { peer_->SetBlendMotionValue(adj_.get_value()/100); };
	Gtk::Adjustment adj_;	
	StrokePtr peer_;
};


StrokeMotionWidget::StrokeMotionWidget(StrokePtr peer) : peer_(peer)
{
	holder_.set_shadow_type(Gtk::SHADOW_NONE);

	velWidget_ = manage(new StrokeMotionVelCtrl(peer));
	StrokeMotionCueCombo *wCue = manage(new StrokeMotionCueCombo(peer));
	Gtk::HBox *box = manage(new Gtk::HBox);



	box->pack_start(*wCue,Gtk::PACK_SHRINK);
	box->pack_start(*velWidget_);

	pack_start(*box,Gtk::PACK_SHRINK);
	pack_start(holder_,Gtk::PACK_SHRINK);
	
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

	Gtk::VBox *box = manage(new Gtk::VBox);
	holder_.add(*box);
	if (cue == Stroke::motion_cue_none)
	{
	}
	else if (cue == Stroke::motion_cue_inherent)
	{
	}
	else if (cue == Stroke::motion_cue_superposed)
	{
		Gtk::HBox *hbox = manage(new Gtk::HBox);
		hbox->pack_start(*manage(new StrokeMinMotionWidthCtrl(peer_)));
		hbox->pack_start(*manage(new StrokeSuperposedMotionRatioCtrl(peer_)));
		box->pack_start(*hbox);
		box->pack_start(*manage(new GratingPitchWidget(peer_->GetMotionTexture())),Gtk::PACK_SHRINK);
		box->pack_start(*manage(new OscillatorWidget(peer_->GetMotionOscillator())),Gtk::PACK_SHRINK);
	}
	else if (cue == Stroke::motion_cue_suppressed)
	{
		box->pack_start(*manage(new StrokeMinMotionWidthCtrl(peer_)),Gtk::PACK_SHRINK);
		box->pack_start(*manage(new GratingPitchWidget(peer_->GetMotionTexture())),Gtk::PACK_SHRINK);
		box->pack_start(*manage(new OscillatorWidget(peer_->GetMotionOscillator())),Gtk::PACK_SHRINK);
	}
	else if (cue == Stroke::motion_cue_vib)
	{
		box->pack_start(*manage(new StrokeMinMotionWidthCtrl(peer_)),Gtk::PACK_SHRINK);
		box->pack_start(*manage(new GratingPitchWidget(peer_->GetMotionTexture())),Gtk::PACK_SHRINK);
		Gtk::Frame *frame = manage(new Gtk::Frame("frequency"));
		frame->add(*manage(new OscillatorFreqCtrl(peer_->GetMotionOscillator())));
		box->pack_start(*frame,Gtk::PACK_SHRINK);
	}
	else if (cue == Stroke::motion_cue_blend)
	{
		box->pack_start(*manage(new StrokeMinMotionWidthCtrl(peer_)),Gtk::PACK_SHRINK);
		box->pack_start(*manage(new GratingPitchWidget(peer_->GetMotionTexture())),Gtk::PACK_SHRINK);
		box->pack_start(*manage(new StrokeBlendMotionValueCtrl(peer_)),Gtk::PACK_SHRINK);
	}
	show_all_children();
}

} // namespace graphics
} // namespace latero

