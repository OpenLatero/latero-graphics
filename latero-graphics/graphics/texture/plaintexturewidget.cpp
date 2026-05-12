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
#include "plaintexture.h"
#include "texturewidget.h"
#include "../../gtk/numwidget.h"
#include "../../units.h"
#include "../patternpreview.h"
#include "../../oscillatorwidget.h"

namespace latero {
namespace graphics { 

PlainTextureWidget::PlainTextureWidget(PlainTexturePtr peer) :
	Gtk::Box(Gtk::Orientation::HORIZONTAL), peer_(peer)
{
	auto sidebox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	auto textureInvertCtrl = Gtk::make_managed<TextureInvertCtrl>(peer);
	auto textureAmplitudeCtrl = Gtk::make_managed<TextureAmplitudeCtrl>(peer);
	auto oscillatorWidget = Gtk::make_managed<OscillatorWidget>(peer->GetOscillator());
	auto patternPreview = Gtk::make_managed<PatternPreview>(peer);

	textureAmplitudeCtrl->set_vexpand();
	oscillatorWidget->set_hexpand();

	sidebox->append(*textureInvertCtrl);
	sidebox->append(*textureAmplitudeCtrl);
	append(*sidebox);
	append(*oscillatorWidget);
	append(*patternPreview);
}

} // namespace graphics
} // namespace latero
