#pragma once

#include <laterographics/generator.h>

typedef std::shared_ptr<class VibrationGenerator> VibrationGeneratorPtr;

class VibrationGenerator : public latero::graphics::Generator
{
public:
	static VibrationGeneratorPtr Create(const latero::Tactograph *dev);

	// This method creates the custom VibrationGeneratorWidget associated with VibrationGenerator.
	virtual Gtk::Widget *CreateWidget(latero::graphics::GeneratorPtr ptr);

	inline double GetFrequency() const { LATERO_GRAPHICS_GUARD; return GetFrequency_(); }
	inline double GetFrequency_() const { return frequency_; }
	inline void SetFrequency(double f) { LATERO_GRAPHICS_GUARD; SetFrequency_(f); }
	inline void SetFrequency_(double f) { frequency_ = f; SetLastModified_(); }

protected:
	VibrationGenerator(const latero::Tactograph *dev);
	virtual void DoRender_(const latero::graphics::State *state, latero::BiasedImg &frame);

	double frequency_ = 1.0; // in Hertz;
};
