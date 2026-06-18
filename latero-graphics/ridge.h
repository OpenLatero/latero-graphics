#pragma once

#include <memory>
#include <string>
#include <vector>

#include "modifiable.h"
#include "xml.h"

namespace latero::graphics {

typedef std::shared_ptr<class Ridge> RidgePtr;

class Ridge : public Modifiable
{
public:
	/** defaults */
    constexpr static const double width_default = 0.5;
    constexpr static const double edgeWidth_min = 0.1;
    constexpr static const double edgeWidth_default = 0.2;
    constexpr static const double txAmp_default = 0;
	static const unsigned int txNbCycles_default = 3;
	static const bool txEnable_default = false;

	static RidgePtr Create(double edgeWidth = edgeWidth_default)
	{
		return RidgePtr(new Ridge(edgeWidth));
	}

	void AppendXML(XMLOutputNode &root) const;
	void LoadXML(const XMLInputNode &root);

	Ridge operator=(const Ridge& p)
	{
		LATERO_GRAPHICS_GUARD;
		if (&p == this) return *this;
		width_ = p.width_;
		edgeWidth_ = p.edgeWidth_;
		txAmp_ = p.txAmp_;
		txNbCycles_ = p.txNbCycles_;
		return *this;
	};

	void Copy(const RidgePtr p)
	{
		LATERO_GRAPHICS_GUARD;
		width_ = p->GetWidth();
		edgeWidth_ = p->GetEdgeWidth();
		txAmp_ = p->GetTxAmp();
		txNbCycles_ = p->GetTxNbCycles();
	};

	/**
	 * @param d distance from center in mm
	 * @return a value between 0 and 1
	 * @note non-locking
	 */
	float GetValue_(float d);

	/**
	 * @return shape, without texture, at distance d 
	 */
	double GetShape_(double d) { return GetShape_(d,width_,edgeWidth_); }
	static double GetShape_(double d, double width, double edge);

	inline double GetWidth() const 		{ LATERO_GRAPHICS_GUARD; return GetWidth_(); }
	inline double GetWidth_() const 	{ return width_; }
	inline void SetWidth(double v) 		{ LATERO_GRAPHICS_GUARD; SetWidth_(v); }
	inline void SetWidth_(double v) 	{ width_=v; SetLastModified_(); }

	inline double GetEdgeWidth() const	{ LATERO_GRAPHICS_GUARD; return GetEdgeWidth_(); }
	inline double GetEdgeWidth_() const	{ return edgeWidth_; }
	inline void SetEdgeWidth(double v) 	{ LATERO_GRAPHICS_GUARD; SetEdgeWidth_(v); }
	inline void SetEdgeWidth_(double v) 	{ edgeWidth_ = fmax(edgeWidth_min,v); SetLastModified_(); }

	inline bool GetTxEnable() const 	{ LATERO_GRAPHICS_GUARD; return txEnable_; }
	inline void SetTxEnable(bool v) 	{ LATERO_GRAPHICS_GUARD; SetLastModified_(); txEnable_ = v; }

	inline double GetTxAmp() const 		{ LATERO_GRAPHICS_GUARD; return txAmp_; }
	inline void SetTxAmp(float v) 		{ LATERO_GRAPHICS_GUARD; SetLastModified_(); txAmp_ = v; }

	inline unsigned int GetTxNbCycles() const	{ LATERO_GRAPHICS_GUARD; return txNbCycles_; }
	inline void SetTxNbCycles(unsigned int v) 	{ LATERO_GRAPHICS_GUARD; SetLastModified_(); txNbCycles_ = v; }

protected:
	Ridge(double edgeWidth);

	double width_;			// entire width of the ridge (both sides)
	double edgeWidth_; 		// length of the edge in mm
	bool txEnable_;			// enable or disable texture
	double txAmp_; 			// amplitude between 0.0 and 1.0
	unsigned int txNbCycles_; 	// nb of full cycles per side (a half cycle is added)
};

} // namespace

