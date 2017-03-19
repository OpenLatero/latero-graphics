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

#ifndef LATERO_GRAPHICS_PLANAR_OBJECT_SET_H
#define LATERO_GRAPHICS_PLANAR_OBJECT_SET_H

#include "pattern.h"
#include "groupfwd.h"
#include "../namedid.h"
#include "modulator/modulatorfwd.h"

namespace latero {
namespace graphics { 

// TODO: turn into a template for different types of patterns?
class Group : virtual public Pattern
{
public:
	typedef NamedID Operation;
	typedef NamedIDSet OperationSet;
	static const Operation op_multiply, op_add, op_max, op_blend, op_over, op_reactive;


	static GroupPtr Create(const latero::Tactograph *dev)
	{
		return GroupPtr(new Group(dev));
	}
	static GroupPtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return GroupPtr(new Group(dev,node));
	}
	virtual ~Group() {}

	virtual void LoadXML(const XMLInputNode &root);
	void LoadOperation(const XMLInputNode &root);
	void LoadPatterns(const XMLInputNode &root);

	virtual void AppendXML(XMLOutputNode &root) const;
	void AppendOperation(XMLOutputNode &root) const;
	void AppendPatterns(XMLOutputNode &root) const;

	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t);

	virtual bool UpdateVisible_(const State *state);
	virtual void ClearVisible_();

	virtual double DoRender_(const ActuatorState &state);
	virtual double DoRenderShadow_(const ActuatorState &state);
	virtual std::string GetName();
	Gtk::Widget *CreateWidget(PatternPtr ptr);

	std::vector<PatternPtr> GetPatterns() const;
	void InsertPattern(PatternPtr obj);
	void InsertPatternAfter(PatternPtr newPattern, PatternPtr curPattern);
	void InsertPatternBefore(PatternPtr newPattern, PatternPtr curPattern);
	void RemovePattern(PatternPtr obj);
	inline void ClearPatterns() { LATERO_GRAPHICS_GUARD; ClearPatterns_(); }
	void ClearPatterns_();
	void ReplacePattern(PatternPtr oldPattern, PatternPtr newPattern);
	void ReplacePattern(int pos, PatternPtr tx);
	void MoveDownPattern(PatternPtr pattern);
	void MoveUpPattern(PatternPtr pattern);

	OperationSet GetOperations() const { return opSet_; };
	Operation GetOperation() const	{ LATERO_GRAPHICS_GUARD; return op_; }
	void SetOperation(const Operation &op);
	void SetOperation(std::string label);

	// controls how this appears in GroupWidget
	virtual bool ChildrenArePublic() { return true; }

	void SetReactiveMod(ModulatorPtr mod);
	ModulatorPtr GetReactiveMod() const { LATERO_GRAPHICS_GUARD; return reactiveMod_; }

	virtual void PlayAudio(AudioDevicePtr dev, Point pos);
    virtual bool OnKeyPress(GdkEventKey* event);

	// only for reactive mode
	typedef enum { ill_mode_mix, ill_mode_first, ill_mode_second } ill_mode;
	void SetIllMode(ill_mode mode) { LATERO_GRAPHICS_GUARD; illMode_ = mode; }
	ill_mode GetIllMode() { LATERO_GRAPHICS_GUARD; return illMode_; }
    
    
    virtual void OnButtonEvent_(const ButtonEvent &event);

protected:
	Group(const latero::Tactograph *dev);
	Group(const latero::Tactograph *dev, const XMLInputNode &node);

	std::vector<PatternPtr> objects_;

	Operation op_;	// todo: add to xml
	OperationSet opSet_;

	ModulatorPtr reactiveMod_; // velocity-based modulator for op_reactive
	ill_mode illMode_;
};

} // namespace graphics
} // namespace latero

#endif
