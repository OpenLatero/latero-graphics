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

#ifndef LATERO_GRAPHICS_PLANAR_MODULATOR_PREVIEW
#define LATERO_GRAPHICS_PLANAR_MODULATOR_PREVIEW

#include "modulatorfwd.h"
#include <gtkmm/eventbox.h>
#include <gtkmm/image.h>
#include <gtkmm/uimanager.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero {
namespace graphics { 

class ModulatorPreview : public Gtk::EventBox
{
public:
	ModulatorPreview(ModulatorPtr peer);
	virtual ~ModulatorPreview() {}
protected:
	void CreatePopupMenu();
	void OnSave();
	void OnSaveAs();
	bool OnClick(GdkEventButton* event);
	bool OnTimer();
	void Refresh();

	boost::posix_time::ptime refreshTime_;
	Gtk::Image img_;
	Glib::RefPtr<Gtk::UIManager> uiManager_;
	ModulatorPtr peer_;
};

} // namespace graphics
} // namespace latero

#endif

