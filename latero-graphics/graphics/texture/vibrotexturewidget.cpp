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
#include <gtkmm/spinbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include "vibrotexture.h"
#include "texturewidget.h"
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "../../graphics/patternpreview.h"

namespace latero {
namespace graphics { 

class VibroTextureFreqCtrl : public Gtk::VBox
{
public:
	VibroTextureFreqCtrl(VibroTexturePtr peer) :
		adj_(peer->GetFreq(),Oscillator::freq_min,Oscillator::freq_max), peer_(peer)
	{
		pack_start(*manage(new gtk::HNumWidget("frequency", adj_,1, units::hz)));
		adj_.signal_value_changed().connect(sigc::mem_fun(*this, &VibroTextureFreqCtrl::OnChanged));
	}
protected:
	void OnChanged() { peer_->SetFreq(adj_.get_value()); }
	Gtk::Adjustment adj_;
	VibroTexturePtr peer_;
};


VibroTextureWidget::VibroTextureWidget(VibroTexturePtr peer) :
	peer_(peer)
{
	Gtk::VBox *sidebox = manage(new Gtk::VBox);
	sidebox->pack_start(*manage(new TextureInvertCtrl(peer)), Gtk::PACK_SHRINK);
	sidebox->pack_start(*manage(new TextureAmplitudeCtrl(peer)));
	pack_start(*sidebox, Gtk::PACK_SHRINK);
	pack_start(*manage(new VibroTextureFreqCtrl(peer)));
	pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);
}

} // namespace graphics
} // namespace latero
