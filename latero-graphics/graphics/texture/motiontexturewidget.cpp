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

class CueTypeDropDown : public Gtk::Box, MotionTextureCtrl
{
public:
	CueTypeDropDown(MotionTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::HORIZONTAL),
		MotionTextureCtrl(peer),
		list_(Gtk::StringList::create({})),
		dropDown_(list_)
	{
		for (const auto& op : peer->GetCueTypes())
			list_->append(op.label);
		Glib::ustring target = peer->GetCueType().label;
		for (guint i = 0; i < list_->get_n_items(); ++i)
			if (list_->get_string(i) == target) { dropDown_.set_selected(i); break; }
		dropDown_.property_selected().signal_changed().connect(sigc::mem_fun(*this, &CueTypeDropDown::OnChange));
		append(dropDown_);
	};
	virtual ~CueTypeDropDown() {};
	sigc::signal<void()>& SignalChanged() { return signalChanged_; };
private:
	Glib::RefPtr<Gtk::StringList> list_;
	Gtk::DropDown dropDown_;
	sigc::signal<void()> signalChanged_;
	void OnChange() { peer_->SetCueType(std::string(list_->get_string(dropDown_.get_selected()))); signalChanged_(); };
};

class DirectionCtrl : public Gtk::Box, MotionTextureCtrl
{
public:
	DirectionCtrl(MotionTexturePtr peer) : Gtk::Box(Gtk::Orientation::HORIZONTAL), MotionTextureCtrl(peer),
		adj_(Gtk::Adjustment::create(peer->GetDirection(),0,360))
	{
		append(*Gtk::make_managed<gtk::HNumWidget>("direction", adj_, 0, units::degree));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &DirectionCtrl::OnChanged));
	}
	virtual ~DirectionCtrl() {};
protected:
	void OnChanged() { peer_->SetDirection(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

class VelocityCtrl : public Gtk::Box, MotionTextureCtrl
{
public:
	VelocityCtrl(MotionTexturePtr peer) : Gtk::Box(Gtk::Orientation::HORIZONTAL), MotionTextureCtrl(peer),
		adj_(Gtk::Adjustment::create(peer->GetVelocity(),0,100))
	{
		append(*Gtk::make_managed<gtk::HNumWidget>("velocity", adj_, 1, units::mm_per_sec));
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &VelocityCtrl::OnChanged));
	}
	virtual ~VelocityCtrl() {};
protected:
	void OnChanged() { peer_->SetVelocity(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
};

}; // namespace MotionTextureCtrls


MotionTextureWidget::MotionTextureWidget(MotionTexturePtr peer) :
	peer_(peer),
	txWidget_(peer->GetTexture())
{
	using namespace MotionTextureCtrls;
	auto motionPage = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	box->append(*Gtk::make_managed<CueTypeDropDown>(peer));
	box->append(*Gtk::make_managed<DirectionCtrl>(peer));
	box->append(*Gtk::make_managed<VelocityCtrl>(peer));
	motionPage->append(*box);
	box->set_hexpand();
	motionPage->append(*Gtk::make_managed<PatternPreview>(peer->GetMotionTexture()));

	append_page(txWidget_, "texture");
	append_page(*motionPage, "motion cue");

	txWidget_.SignalTextureChanged().connect(sigc::mem_fun(*this, &MotionTextureWidget::OnTextureChanged));;
}

void MotionTextureWidget::OnTextureChanged()
{
	peer_->SetTexture(txWidget_.GetTexture());
}

} // namespace graphics
} // namespace latero
