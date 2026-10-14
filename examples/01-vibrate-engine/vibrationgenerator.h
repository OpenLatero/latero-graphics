#pragma once

#include <laterographics/generator.h>

// Optional: create a short name for a smart pointer to a VibrationGenerator.
typedef std::shared_ptr<class VibrationGenerator> VibrationGeneratorPtr;

class VibrationGenerator : public latero::graphics::Generator
{
public:
	// This creates a smart pointer to a new VibrationGenerator that will be deleted automatically
	// once no one holds a pointer to it anymore.
	static VibrationGeneratorPtr Create(const latero::Tactograph *dev);

	// We define set/get methods for parameters, in this case frequency_. Get/SetFrequency() are thread-safe
	// methods that use LATERO_GRAPHICS_GUARD to prevent concurrent modifications between threads. Get/SetFrequency_()
	// are not thread-safe and should only be used when LATERO_GRAPHICS_GUARD is already locked, such as in DoRender_().
	// SetLastModified_() is called in SetFrequency_() to trigger fading and prevent abrupt rendering changes.
	inline double GetFrequency() const { LATERO_GRAPHICS_GUARD; return GetFrequency_(); }
	inline double GetFrequency_() const { return frequency_; }
	inline void SetFrequency(double f) { LATERO_GRAPHICS_GUARD; SetFrequency_(f); }
	inline void SetFrequency_(double f) { frequency_ = f; SetLastModified_(); }

protected:
	// The constructor is declared 'protected' so that Create() is the only way to instanciate a VibrationGenerator.
	VibrationGenerator(const latero::Tactograph *dev);

	// This method is defined in latero::graphics::Generator and is called by TactileEngine to render
	// a frame for the Latero base on its current state.
	virtual void DoRender_(const latero::graphics::State *state, latero::BiasedImg &frame);

	double frequency_ = 1.0; // in Hertz;
};
