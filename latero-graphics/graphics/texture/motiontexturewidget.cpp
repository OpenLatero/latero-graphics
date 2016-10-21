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

#include "motiontexturewidget.h"
#include "motiontexture.h"
#include "../../gtk/numwidget.h"
#include <gtkmm.h>
#include "../../pointwidget.h"
#include "../../graphics/patternpreview.h"

namespace latero {
namespace graphics { 

namespace MotionTextureCtrls {

class MotionTextureCtrl
{
public:
	MotionTextureCtrl(MotionTexturePtr peer) : peer_(peer) {}
	virtual ~MotionTextureCtrl() {};
protected:
	MotionTexturePtr peer_;
};

class CueTypeCombo : public Gtk::ComboBoxText, MotionTextureCtrl
{
public:
	CueTypeCombo(MotionTexturePtr peer) : MotionTextureCtrl(peer)
	{
		MotionTexture::CueTypeSet ops = peer->GetCueTypes();
		for (unsigned int i=0; i<ops.size(); ++i)
			append_text(ops[i].label);
		set_active_text(peer->GetCueType().label);
		signal_changed().connect(sigc::mem_fun(*this, &CueTypeCombo::OnChange));
	};
	virtual ~CueTypeCombo() {};
	sigc::signal<void> SignalChanged() { return signalChanged_; };
private:
	sigc::signal<void> signalChanged_;
	void OnChange() { peer_->SetCueType(get_active_text()); signalChanged_(); };
};

class DirectionCtrl : public Gtk::HBox, MotionTextureCtrl
{
public:
	DirectionCtrl(MotionTexturePtr peer) : MotionTextureCtrl(peer),
		adj_(peer->GetDirection(),0,360)
	{
		add(*manage(new gtk::HNumWidget("direction", adj_, 0, units::degree)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &DirectionCtrl::OnChanged));
	}
	virtual ~DirectionCtrl() {};
protected:
	void OnChanged() { peer_->SetDirection(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

class VelocityCtrl : public Gtk::HBox, MotionTextureCtrl
{
public:
	VelocityCtrl(MotionTexturePtr peer) : MotionTextureCtrl(peer),
		adj_(peer->GetVelocity(),0,100)
	{
		add(*manage(new gtk::HNumWidget("velocity", adj_, 1, units::mm_per_sec)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &VelocityCtrl::OnChanged));
	}
	virtual ~VelocityCtrl() {};
protected:
	void OnChanged() { peer_->SetVelocity(adj_.get_value()); }
	Gtk::Adjustment adj_;
};

}; // namespace MotionTextureCtrls


MotionTextureWidget::MotionTextureWidget(MotionTexturePtr peer) :
	peer_(peer),
	txWidget_(peer->GetTexture())
{
	using namespace MotionTextureCtrls;
	Gtk::HBox *motionPage = manage(new Gtk::HBox);
	Gtk::VBox *box = manage(new Gtk::VBox);
	box->pack_start(*manage(new CueTypeCombo(peer)), Gtk::PACK_SHRINK);
	box->pack_start(*manage(new DirectionCtrl(peer)), Gtk::PACK_SHRINK);
	box->pack_start(*manage(new VelocityCtrl(peer)), Gtk::PACK_SHRINK);
	motionPage->pack_start(*box);
	motionPage->pack_start(*manage(new PatternPreview(peer->GetMotionTexture())), Gtk::PACK_SHRINK);

	append_page(txWidget_, "texture");
	append_page(*motionPage, "motion cue");
	show_all_children();

	txWidget_.SignalTextureChanged().connect(sigc::mem_fun(*this, &MotionTextureWidget::OnTextureChanged));;
}

void MotionTextureWidget::OnTextureChanged()
{
	peer_->SetTexture(txWidget_.GetTexture());
}

} // namespace graphics
} // namespace latero
