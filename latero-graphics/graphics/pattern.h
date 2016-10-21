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

#ifndef LATERO_GRAPHICS_PATTERN_H
#define LATERO_GRAPHICS_PATTERN_H

#include <latero/tactograph.h>
#include <gtkmm/widget.h>
#include <audiere.h>
#include "patternfwd.h"
#include "../modifiable.h"
#include "../state.h"
#include "../xml.h"
#include "../buttonevent.h"

namespace latero {
namespace graphics { 

/** 
 * A Pattern is a basic tactile graphics element which knows how to render itself,
 * both visually and haptically.
 *
 * @todo This ended up looking a whole lot like a PositionGen. Merge?
 */
class Pattern : public Modifiable
{
public:

	/** visualization mode */
	typedef enum { 
		viz_abstract, /*!< Abstract illustration. */
		viz_shadow, /*!< Shadow rendering map. */
		viz_deflection, /*!< Deflection rendering map. */
		viz_deflection_and_vibration /*!< Deflection rendering map with vibration as white noise. */
	} VizMode;

	/** Create Pattern from an XML node */
	static PatternPtr Create(const latero::Tactograph *dev, const XMLInputNode &node);

	/** Create Pattern from an XML file */
	static PatternPtr Create(const latero::Tactograph *dev, std::string xmlFile);

	/** Destructor */
	virtual ~Pattern() {}

	/**
	 * Compute the deflection of a single actuator given its state. By convention, 
	 * 0 corresponds to a deflection to the right, and 1 to the left.
	 */
	double Render_(const ActuatorState &state, bool ignoreVisibility=false);
	
	/**
	 * Compute a pattern's "shadow" for overwriting an underlying pattern.
	 * @todo This computation is very similar to Render_ in most cases so it
	 * should be possible to reduce computations somehow.
	 */
	double RenderShadow_(const ActuatorState &state, bool ignoreVisibility=false);

	/**
	 * Obtain an abstract Cairo::Pattern representing the tactile pattern. This is a vector graphics drawing
	 * which approximates the rendering but is computed faster than a deflection map.
	 */
	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration timeElapsed) = 0;

	/**
	 * Obtain an illustration of the pattern according to the vizualization mode.
	 * @todo Harmonize with similar method in Generator.
	 */
	Glib::RefPtr<Gdk::Pixbuf> GetVisualization(uint w, boost::posix_time::time_duration t, VizMode mode, double velMag = 0, double velDir = 0);
	
	/** 
	 * @return name of pattern (currently Type descriptor) 
	 * @todo This should be implemented using a member variable rather than a virtual method.
	 * @todo This name should be selected by the user as a better descriptor of the Pattern.
	 */
	virtual std::string GetName() = 0;

	/** @return widget allowing control of the pattern's parameters */
	virtual Gtk::Widget *CreateWidget(PatternPtr ptr);

	/** Append XML description of Pattern to an existing XML node. */
	virtual void AppendXML(XMLOutputNode &root) const = 0;

	/** Save pattern to an XML file */
	void SaveToFile(std::string xmlFile);

	/**
	 * Updates the visibility of the Pattern based on the current state. This
	 * method notes whether the current Pattern will have an effect on the
	 * rendering of any of the actuators, setting the visible_ member variable
	 * accordingly. This is used to optimize calls to Render_ such that no computation
	 * is made when the Pattern is not visible. This method is normally called
	 * once before actuators are individually rendered, often allowing a single
	 * check to be made instead of one for each actuator.
	 *
	 * @note By default, this method sets visible_ to true such that the pattern
	 * is rendered. Each derived class is responsible for re-implementing this
	 * as appropriate for optimization.
	 */
	virtual bool UpdateVisible_(const State *state);

	/**
	 * Resets the visibility of the Pattern. This is normally called after
	 * rendering the entire frame so that the Pattern can be reset to its
	 * initial state.
	 *
	 * @todo This should not be necessary since the next call to UpdateVisible_ should
	 * clear up the visibility. Unless perhaps this could cause issues with visualizations?
	 * Try and see...
	 */
	virtual void ClearVisible_();

	/** 
	 * Play sound based on current tactograph location.
	 */
	virtual void PlayAudio(audiere::AudioDevicePtr dev, Point pos) {};

	/** 
	 * Handle key press or return false if key is unused.
	 */
	virtual bool OnKeyPress(GdkEventKey* event) { return false; }

	/** @return path of the source XML file, if created from a file */ 
	std::string GetXMLFile() const { LATERO_GRAPHICS_GUARD; return xmlfile_; }

	/** set the path of the source XML file */
	void SetXMLFile(const std::string file)	{ LATERO_GRAPHICS_GUARD; xmlfile_ = file; }

	/** @return pointer to Tactograph */
	inline const latero::Tactograph *Dev() const { return dev_; }

    virtual void OnButtonEvent_(const ButtonEvent &event) {};

protected:
	/** Constructor */
	Pattern(const latero::Tactograph *dev);

	/** Get visibility. */
	inline bool GetVisible_() const { return visible_; }

	/** Set visibility */
	inline void SetVisible_(bool v) { visible_=v; }

	/** This function is called by Render_() once visibility has been taken into account. */
	virtual double DoRender_(const ActuatorState &state) = 0;

	/** This function is called by RenderShadow_() once visibility has been taken into account. */
	virtual double DoRenderShadow_(const ActuatorState &state) = 0;

private:
	/** 
	 * Indicates whether the Pattern affects the rendering of any of the actuators given the current
	 * state of the system. Used for optimization of the tactile rendering.
	 */
	bool visible_;

	/** Pointer to the tactograph device */
	const latero::Tactograph *dev_;

	/** path to the xml source file, if created from a file */
	std::string xmlfile_;
};

} // namespace graphics
} // namespace latero

#endif
