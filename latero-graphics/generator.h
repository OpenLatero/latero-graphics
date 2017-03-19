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

#ifndef LATERO_GRAPHICS_GENERATOR_H
#define LATERO_GRAPHICS_GENERATOR_H

#include <latero/tactograph.h>
#include <gtkmm/widget.h>
#include "generatorfwd.h"
#include "modifiable.h"
#include "gtk/animation.h"
#include "xml.h"
#include "state.h"
#include "buttonevent.h"
#include "audiodevice.h"

namespace latero {
namespace graphics { 

/**
 * @defgroup GeneratorGroup Generators
 *
 * Generators are classes capable of performing tactile rendering operations and of
 * representing their tactile content visually. Generators where initially at the 
 * core of latero-graphics and existed in different variations such as WaveGen which generated
 * gratings. With the refocusing of the library on tactile graphics, most generators 
 * have been eliminated, leaving only base classes Generator and PositionGen, and a 
 * single instantiable generator called Canvas which handles tactile graphics. The
 * Generator remains the class of objects used by TactileEngine to produce renderings. 
 *
 * @todo The name Generator dates back to Jerome and Vincent's early work on Braille
 * and may no longer be appropriate. Perhaps "renderer" or a similar term would be
 * a better fit.
 *
 * @todo There is a significant amount of overlap between a Pattern and a Generator. The 
 * distinction between the two classes should be revisited.
 */

/**
 * The Generator is the base class for objects capable of performing
 * tactile rendering operations. It also acts as a factory for Generators
 * specified through an XML file or node.
 *
 * @ingroup GeneratorGroup 
 *
 * @todo The Generator currently needs to know about every other types of generators so as to act as 
 * a generator factory. This functionality should perhaps be implemented by a separate GeneratorFactory class.
 *
 * @todo By being responsible for creating their own widgets, each Generator is forced to be aware
 * of its corresponding widget class. This functionality should perhaps be implemented by 
 * another factory class or factory methods which would decouple the two classes.
 */
class Generator : public Modifiable
{
public:
	/** Creates a Generator based on an XML file. */
	static GeneratorPtr Create(std::string xmlFile, const latero::Tactograph *dev);

	/** Creates a Generator based on an XML node. */
	static GeneratorPtr Create(const XMLInputNode &node, const latero::Tactograph *dev);

	/** Destructor. */
	virtual ~Generator() {};

	/**
	 * Renders the activation of the actuators based on the current state of the system.
	 * @param current state of the system
	 * @param frame output, activation of each actuator
	 * @note Returning the frame would be cleaner but it would require an additional copy operation.
	 */
	void Render_(const State *state, latero::BiasedImg &frame);

	/** Serialize Generator to an XML file. */
	void SaveToFile(std::string filename) const;

	/** Serialize Generator by appending its information to an XML node. */
	virtual void AppendXML(XMLOutputNode &root) const = 0;

	/** Convenience method that return a pointer to the Tactograph user with this generator. */
	inline const latero::Tactograph *Dev() const { return dev_; }

	/**
	 * Create a widget that controls the parameters of the generator and provides feedback.
	 * @param ptr Shared pointer of the generator.
	 */
	virtual Gtk::Widget *CreateWidget(GeneratorPtr ptr);

	/** 
	 * @return symbolic representation of the virtual surface 
	 * @param w width in pixels
	 * @param t time in ns
	 */
	virtual latero::graphics::gtk::Animation GetIllustration(uint w, boost::posix_time::time_duration t) const;

	/** @return last known frame rendered by this generator */
	latero::BiasedImg GetLatestFrame() const;

	/** @return last known position of the display within the virtual surface (center) */
    inline Point GetDisplayCenter() const { LATERO_GRAPHICS_GUARD; return GetDisplayCenter_(); }

 	/** @return last known position of the display within the virtual surface (center) [non-locking] */
    inline Point GetDisplayCenter_() const { return latestState_.pos; }
    
	/** @return last known display orientation */
	double GetDisplayOrientation() const;

    /** @return last known display speed [non-locking] */
    inline double GetDisplaySpeed_() const { return latestState_.velMag; }
    
	/** Play audio associated with this generator. */
	virtual void PlayAudio(AudioDevicePtr dev);
    
	/**
	 * called when a key is pressed on the keyboard
	 * @return true when key is used by the Generator
	 */	
	virtual bool OnKeyPress(GdkEventKey* event);
    
    /**
     * called when a event related to Latero buttons is detected 
     */
    virtual void OnButtonEvent_(const ButtonEvent &event) {};

protected:
	/** constructor */
	Generator(const latero::Tactograph *dev);

	/** update the state of the Generator */
	void SetLatestState_(const State &state, const latero::BiasedImg &frame);

	/** Implementation of the render function which is defined by derived classes. */
	virtual void DoRender_(const State *state, latero::BiasedImg &frame) = 0;

private:
	/** pointer to the Tactograph device */
	const latero::Tactograph *dev_;

	/** latest frame generated by this generator */
	latero::BiasedImg latestFrame_;

	/** latest state of the Tactograph used to render the latest frame */
	State latestState_;
};

} // namespace graphics
} // namespace latero

#endif
