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

#ifndef LATERO_GRAPHICS_TOGGLE_H
#define LATERO_GRAPHICS_TOGGLE_H

#include "pattern.h"
#include "togglefwd.h"

namespace latero {
namespace graphics { 

class Toggle : virtual public Pattern
{
public:
	static TogglePtr Create(const latero::Tactograph *dev)
	{
		return TogglePtr(new Toggle(dev));
	}
	static TogglePtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return TogglePtr(new Toggle(dev,node));
	}
	static TogglePtr Create(const latero::Tactograph *dev, PatternPtr p1, PatternPtr p2)
	{
		return TogglePtr(new Toggle(dev,p1,p2));
	}

	virtual ~Toggle() {}

	virtual void LoadXML(const XMLInputNode &root);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);

	virtual double DoRender_(const ActuatorState &state);
	virtual double DoRenderShadow_(const ActuatorState &state);
	virtual std::string GetName();
	Gtk::Widget *CreateWidget(PatternPtr ptr);

	virtual void PlayAudio(AudioDevicePtr dev, Point pos);
    
	PatternPtr GetCurrentPattern() { LATERO_GRAPHICS_GUARD; return curPattern_; }

	void DoToggle();
	void DoToggle_();
	//virtual bool OnKeyPress(GdkEventKey* event);
    virtual void OnButtonEvent_(const ButtonEvent &event);
    
	PatternPtr GetFirstPattern() { LATERO_GRAPHICS_GUARD; return patterns_[0]; }
	PatternPtr GetSecondPattern() { LATERO_GRAPHICS_GUARD; return patterns_[1]; }

    // true when second pattern is selected
    inline bool IsToggled_() const { return toggle_; }
    inline bool IsToggled() { LATERO_GRAPHICS_GUARD; return IsToggled_(); }
    
    inline long GetToggleCount_() const { return toggleCount_; }
    inline long GetToggleCount() { LATERO_GRAPHICS_GUARD; return GetToggleCount_(); }
    
protected:
	Toggle(const latero::Tactograph *dev);
	Toggle(const latero::Tactograph *dev, const XMLInputNode &node);
	Toggle(const latero::Tactograph *dev, PatternPtr p1, PatternPtr p2);

	PatternPtr patterns_[2];
	PatternPtr curPattern_;
	bool toggle_;
    long toggleCount_;
};

} // namespace graphics
} // namespace latero

#endif
