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

#ifndef LATERO_GRAPHICS_PLANAR_GRATING_MODULATOR_H
#define LATERO_GRAPHICS_PLANAR_GRATING_MODULATOR_H

#include <string>
#include <math.h>
#include "gratingmodulatorfwd.h"
#include "modifiable.h"
#include "xml.h"
#include "units.h"
#include "namedid.h"

namespace latero {
namespace graphics { 

class GratingModulator : public Modifiable
{
public:
	typedef NamedID Mode;
	typedef NamedIDSet ModeSet;
	static const Mode mode_slope, mode_peak;

	static GratingModulatorPtr Create(std::string units="")
	{
		return GratingModulatorPtr(new GratingModulator(units));
	}

	virtual void LoadXML(const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const;

	inline std::string GetUnits() const  { LATERO_GRAPHICS_GUARD; return units_; }
	inline void SetUnits(std::string v) { LATERO_GRAPHICS_GUARD; units_=v; }

	double ModulatePosition_(double pos);


	void SetEnable(bool v) { LATERO_GRAPHICS_GUARD; enable_=v; SetLastModified_(); }
	bool GetEnable() const { LATERO_GRAPHICS_GUARD; return enable_; }
	bool GetEnable_() const { return enable_; }

	// target multiplication factor for spatial frequency
	void SetFactor(double v) { LATERO_GRAPHICS_GUARD; factor_=v; SetLastModified_(); }
	double GetFactor() const { LATERO_GRAPHICS_GUARD; return GetFactor_(); }
	inline double GetFactor_() const { return factor_; }

	// length of transition
	void SetLength(double v) { LATERO_GRAPHICS_GUARD; length_=v; SetLastModified_(); }
	double GetLength() const { LATERO_GRAPHICS_GUARD; return length_; }

	// location of modulation
	void SetPosition(double v) { LATERO_GRAPHICS_GUARD; pos_=v; SetLastModified_(); }
	double GetPosition() const { LATERO_GRAPHICS_GUARD; return pos_; }

	// mode
	ModeSet GetModes() const { return modeSet_; };
	Mode GetMode() const { LATERO_GRAPHICS_GUARD; return mode_; }
	void SetMode(const Mode &m) { LATERO_GRAPHICS_GUARD; mode_ = m; SetLastModified_(); }
	void SetMode(std::string label) { Mode *m = modeSet_.Get(label); if (m) SetMode(*m); }

protected:

	GratingModulator(std::string unit);
	std::string units_; // unit of distance (for displaying to user)
	bool enable_;
	double length_; // length of the transition
	double factor_; // target factor
	double pos_;
	Mode mode_;
	ModeSet modeSet_;
};

} // namespace graphics
} // namespace latero

#endif
