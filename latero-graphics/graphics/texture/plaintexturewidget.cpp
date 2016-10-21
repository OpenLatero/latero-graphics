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

#include "plaintexturewidget.h"
#include <gtkmm/spinbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include "plaintexture.h"
#include "texturewidget.h"
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "../patternpreview.h"
#include "../../oscillatorwidget.h"

namespace latero {
namespace graphics { 

PlainTextureWidget::PlainTextureWidget(PlainTexturePtr peer) :
	peer_(peer)
{
	Gtk::VBox *sidebox = manage(new Gtk::VBox);
	sidebox->pack_start(*manage(new TextureInvertCtrl(peer)), Gtk::PACK_SHRINK);
	sidebox->pack_start(*manage(new TextureAmplitudeCtrl(peer)));
	pack_start(*sidebox, Gtk::PACK_SHRINK);

	pack_start(*manage(new OscillatorWidget(peer->GetOscillator())));
	pack_start(*manage(new PatternPreview(peer)), Gtk::PACK_SHRINK);
}

} // namespace graphics
} // namespace latero
