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

#include "vibrotexturewidget.h"
#include "vibrotexture.h"
#include "texturewidget.h"
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "../../graphics/patternpreview.h"

namespace latero {
namespace graphics { 

class VibroTextureFreqCtrl : public Gtk::Box
{
public:
	VibroTextureFreqCtrl(VibroTexturePtr peer) :
		Gtk::Box(Gtk::Orientation::VERTICAL), adj_(Gtk::Adjustment::create(peer->GetFreq(),Oscillator::freq_min,Oscillator::freq_max)), peer_(peer)
	{
		auto widget = Gtk::make_managed<gtk::HNumWidget>("frequency", adj_,1, units::hz);
		append(*widget);
		widget->set_vexpand();
		adj_->signal_value_changed().connect(sigc::mem_fun(*this, &VibroTextureFreqCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetFreq(adj_->get_value()); }
    Glib::RefPtr<Gtk::Adjustment> adj_;
	VibroTexturePtr peer_;
};


VibroTextureWidget::VibroTextureWidget(VibroTexturePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	auto sidebox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto textureInvertCtrl = Gtk::make_managed<TextureInvertCtrl>(peer);
	auto textureAmplitudeCtrl = Gtk::make_managed<TextureAmplitudeCtrl>(peer);
	auto vibroTextureFreqCtrl = Gtk::make_managed<VibroTextureFreqCtrl>(peer);
	auto patternPreview = Gtk::make_managed<PatternPreview>(peer);

	textureAmplitudeCtrl->set_vexpand();
	vibroTextureFreqCtrl->set_hexpand();

	sidebox->append(*textureInvertCtrl);
	sidebox->append(*textureAmplitudeCtrl);
	append(*sidebox);
	append(*vibroTextureFreqCtrl);
	append(*patternPreview);
}

} // namespace graphics
} // namespace latero
