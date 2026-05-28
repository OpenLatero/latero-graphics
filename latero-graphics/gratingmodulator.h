#pragma once

#include <string>
#include <math.h>
#include "gratingmodulatorfwd.h"
#include "modifiable.h"
#include "xml.h"
#include "units.h"
#include "namedid.h"

namespace latero::graphics {

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

} // namespace

