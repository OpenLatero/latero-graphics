#pragma once

#include <chrono>
#include "engine.h"
#include "generator.h"
#include "audiodevice.h"
#include <latero/tactograph.h>

namespace latero::graphics {

class AudioEngine : public Engine
{
public:
	AudioEngine(latero::Tactograph *dev, double updateRate);
	virtual ~AudioEngine();
	virtual void EntryPoint();
	void SetGenerator(GeneratorPtr gen);
	void ResetGenerator();
	inline const latero::Tactograph *Dev() const { return dev_; }
private:
	latero::Tactograph *dev_;
	GeneratorPtr gen_;
	const std::chrono::microseconds period_;
    AudioDevicePtr audioDevPtr_;
};

} // namespace
