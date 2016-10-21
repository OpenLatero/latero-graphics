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

#ifndef LATERO_GRAPHICS_MODIFIABLE_H
#define LATERO_GRAPHICS_MODIFIABLE_H

#include <vector>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include "mutex.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero {
namespace graphics { 

class Modifiable;
typedef boost::shared_ptr<Modifiable> ModifiablePtr;

/**
 * A Modifiable class keeps track of the last time it was modified. If a Modifiable object
 * is composed of other Modifiable objects, those can be added with AddModifiableChild(...)
 * so that their modification time can be take into account within the parent Modifiable.
 */
class Modifiable
{
public:
	Modifiable()
	{
		lastModified_ = boost::posix_time::microsec_clock::universal_time();
	}

	virtual ~Modifiable()
	{
	}

	inline void SetLastModified() {	LATERO_GRAPHICS_GUARD; SetLastModified_(); }
	inline void SetLastModified_()
	{
		lastModified_ = boost::posix_time::microsec_clock::universal_time();
	}

	inline boost::posix_time::ptime GetLastModified() { LATERO_GRAPHICS_GUARD;	return GetLastModified_(); }
	inline boost::posix_time::ptime GetLastModified_()
	{
		boost::posix_time::ptime t = lastModified_;
		for (unsigned int i=0; i<children_.size(); ++i)
		{
			boost::posix_time::ptime tgen = children_[i]->GetLastModified_();
			if (t < tgen) t = tgen;
		}
		for (unsigned int i=0; i<schildren_.size(); ++i)
		{
			boost::posix_time::ptime tgen = schildren_[i]->GetLastModified_();
			if (t < tgen) t = tgen;
		}
		return t;
	}

	inline void AddModifiableChild(ModifiablePtr mod) { LATERO_GRAPHICS_GUARD; AddModifiableChild_(mod); }
	inline void AddModifiableChild_(ModifiablePtr mod)
	{
		schildren_.push_back(mod);
	}

	inline void AddModifiableChild(Modifiable *mod) { LATERO_GRAPHICS_GUARD; AddModifiableChild_(mod); }
	inline void AddModifiableChild_(Modifiable *mod)
	{
		children_.push_back(mod);
	}

	inline void RemoveModifiableChild(ModifiablePtr mod) { LATERO_GRAPHICS_GUARD; RemoveModifiableChild_(mod); }
	inline void RemoveModifiableChild_(ModifiablePtr mod)
	{
		schildren_.erase(
			std::remove(schildren_.begin(),schildren_.end(),mod),
			schildren_.end()
		);
	}

	// Conveniance function: removes oldChild and adds newChild
	inline void ReplaceModifiableChild(ModifiablePtr oldChild, ModifiablePtr newChild)
	{ LATERO_GRAPHICS_GUARD; ReplaceModifiableChild_(oldChild,newChild); } 
	inline void ReplaceModifiableChild_(ModifiablePtr oldChild, ModifiablePtr newChild)
	{
		RemoveModifiableChild_(oldChild);
		AddModifiableChild_(newChild);
	}

	inline void RemoveModifiableChild(Modifiable *mod) { LATERO_GRAPHICS_GUARD; RemoveModifiableChild_(mod); }
	inline void RemoveModifiableChild_(Modifiable *mod)
	{
		children_.erase(
			std::remove(children_.begin(),children_.end(),mod),
			children_.end()
		);
	}

private:
	std::vector<ModifiablePtr> schildren_;
	std::vector<Modifiable*> children_;
	boost::posix_time::ptime lastModified_;
};

} // namespace graphics
} // namespace latero

#endif
