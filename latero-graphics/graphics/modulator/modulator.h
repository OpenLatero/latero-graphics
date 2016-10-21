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

#ifndef LATERO_GRAPHICS_PLANAR_MODULATOR_H
#define LATERO_GRAPHICS_PLANAR_MODULATOR_H

#include "modulatorfwd.h"
#include "../../modifiable.h"
#include <latero/tactograph.h>
#include "../../xml.h"
#include <gtkmm/widget.h>
#include "../../state.h"

namespace latero {
namespace graphics { 

/**
 * A modulator takes as input a number of variables (typically sensor readings) and returns
 * a scalar value to be used as a modulator.
 */
class Modulator : public Modifiable
{
public:
	static ModulatorPtr Create(const latero::Tactograph *dev, const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const = 0;
	virtual ~Modulator() {}
	virtual Gtk::Widget *CreateWidget(ModulatorPtr ptr);
	const latero::Tactograph *Dev() { return dev_; }
	virtual double GetValue_(const ActuatorState &state) = 0;
	virtual Glib::RefPtr<Gdk::Pixbuf> GetIllustration(uint size, double maxVel=100);
protected:
	Modulator(const latero::Tactograph *dev) : dev_(dev) {}
	const latero::Tactograph *dev_; 
};

} // namespace graphics
} // namespace latero

#endif

