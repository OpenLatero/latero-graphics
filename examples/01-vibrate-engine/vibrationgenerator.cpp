#include "vibrationgenerator.h"
#include <laterographics/graphics/canvas.h>
#include <chrono>
#include <cmath>

VibrationGeneratorPtr VibrationGenerator::Create(const latero::Tactograph *dev)
{
	return VibrationGeneratorPtr(new VibrationGenerator(dev));
}

VibrationGenerator::VibrationGenerator(const latero::Tactograph *dev) : latero::graphics::Generator(dev)
{
}

void VibrationGenerator::DoRender_(const latero::graphics::State *state, latero::BiasedImg &frame)
{
	// Compute the current time in seconds, based on the elapsed time since the start of the generator
	double t = std::chrono::duration<double>(state->GetTimeElapsed()).count();

	// Compute the vibration as a sine, based on the current time
	double vib = sin(2*M_PI*t*GetFrequency_());

	latero::RangeImg img(frame.SizeX(), frame.SizeY());
	for (uint y=0; y<img.SizeY(); ++y)
	{
		for (uint x=0; x<img.SizeX(); ++x)
		{
			double value = (y%2==1) ? vib : -vib;
			img.Set(x,y,value);
		}
	}

	// We used a RangeImg, so we now convert it to the expected BiasedImg.
	frame.ConvertFromRange(img);
}
