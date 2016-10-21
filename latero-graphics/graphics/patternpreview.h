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

#ifndef LATERO_GRAPHICS_PATTERN_PREVIEW
#define LATERO_GRAPHICS_PATTERN_PREVIEW

#include <gtkmm/eventbox.h>
#include <gtkmm/image.h>
#include <gtkmm/uimanager.h>
#include "pattern.h"

namespace latero {
namespace graphics { 

class PatternPreview : public Gtk::EventBox
{
public:
	static const Pattern::VizMode viz_mode = Pattern::viz_deflection_and_vibration;
	static const int height = 200;

	PatternPreview(PatternPtr peer);
	virtual ~PatternPreview() {}
protected:
	void CreatePopupMenu();
	void OnSave();
	bool OnClick(GdkEventButton* event);
	bool OnTimer();
	void Refresh();

	boost::posix_time::ptime refreshTime_;
	Gtk::Image img_;
	Glib::RefPtr<Gtk::UIManager> uiManager_;
	PatternPtr peer_;
};

} // namespace graphics
} // namespace latero

#endif
