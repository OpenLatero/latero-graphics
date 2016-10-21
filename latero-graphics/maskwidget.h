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

#ifndef LATERO_GRAPHICS_PLANAR_MASK_WIDGET
#define LATERO_GRAPHICS_PLANAR_MASK_WIDGET

#include <gtkmm/box.h>
#include "maskfwd.h"
#include "generatorfwd.h"
#include <latero/tactograph.h>

namespace latero {
namespace graphics { 

class MaskWidgetCtrl 
{
public:
	MaskWidgetCtrl(MaskPtr peer) : disableSignals_(false), peer_(peer) {}
	virtual ~MaskWidgetCtrl() {}
	void SynchFromPeer()
	{
		bool v = disableSignals_;
		disableSignals_ = true;
		SynchFromPeer_();
		disableSignals_ = v;
	}
	virtual void SynchFromPeer_() {}
protected:
	bool disableSignals_;
    MaskPtr peer_;
};

class MaskWidget : public Gtk::HBox
{
public:
	MaskWidget(MaskPtr peer, const latero::Tactograph *dev);
	virtual ~MaskWidget() {};
protected:
	void SynchFromPeer();
	std::vector<MaskWidgetCtrl*> ctrls_;
};

} // namespace graphics
} // namespace latero

#endif
