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

#ifndef LATERO_GRAPHICS_CANVAS_H
#define LATERO_GRAPHICS_CANVAS_H

#include "canvasfwd.h"
#include "../positiongen.h"
#include "groupfwd.h"
#include "patternfwd.h"

namespace latero {
namespace graphics { 

/**
 * The Canvas is a specialization of the PositionGen, and hence indirectly of Generator, which is used for the rendering of
 * tactile graphics. The actual rendering is done by Pattern objects.
 *
 * @ingroup GeneratorGroup 
 * 
 * @todo A Canvas should ideally hold a single arbitrary Pattern instead of a Group. The use of a Group is a quick hack
 * to simply the CanvasWidget so that patterns can easilly be added or removed. This causes minor inefficiencies when
 * a single pattern is displayed and always shows a Group within the list of patterns even if not needed.
 *
 * @todo This library used to contain many different types of Generators. All that remains is Canvas and two abstract classes,
 * Generator and PositionGen. It may be a good idea to eventually collapse all three into a single class for simplicity. The
 * current organization, however, allows for Generators that operate based on principles other than the Canvas and could 
 * therefore be useful for testing other rendering concepts.
 */
class Canvas : public PositionGen
{
public:
	/** creates an empty Canvas */
	static CanvasPtr Create(const latero::Tactograph *dev);

	/** creates and initializes a canvas based on the content of an XML node */
	static CanvasPtr Create(const latero::Tactograph *dev, const XMLInputNode &node);

	/** destructor */
	virtual ~Canvas();

	/** @return pointer to Group contained in the Canvas */
	GroupPtr GetGroup() const;

	// see Generator
	virtual void AppendXML(XMLOutputNode &root) const;

	// see Generator
	virtual latero::graphics::gtk::Animation GetIllustration(uint w, boost::posix_time::time_duration t) const;

	// see Generator
	virtual Gtk::Widget *CreateWidget(GeneratorPtr ptr);

	// see Generator
	virtual void PlayAudio(AudioDevicePtr dev);
    
	// see Generator
	virtual bool OnKeyPress(GdkEventKey* event);

    // see Generator
    virtual void OnButtonEvent_(const ButtonEvent &event);
    
protected:
	/** constructor */
	Canvas(const latero::Tactograph *dev);

	/** constructor from XML node */
	Canvas(const latero::Tactograph *dev, const XMLInputNode &node);

	// see PositionGen
	virtual void PreRender_(const State *state);

	// see PositionGen
	virtual double Render_(const ActuatorState &state, bool stateless=false);

	// see PositionGen
	virtual void PostRender_(const State *state);

	/** Group of Patterns displayed in the Canvas */
	GroupPtr objects_;
};

} // namespace graphics
} // namespace latero

#endif

