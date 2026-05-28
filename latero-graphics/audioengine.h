#pragma once

#include "engine.h"
#include "generatorfwd.h"
#include "audiodevicefwd.h"
#include <latero/tactograph.h>

namespace latero::graphics {

class AudioEngine : public Engine
{
public:
	AudioEngine(latero::Tactograph *dev, boost::posix_time::time_duration period);
	virtual ~AudioEngine();
	virtual void EntryPoint();
	void SetGenerator(GeneratorPtr gen);
	void ResetGenerator();
	inline const latero::Tactograph *Dev() const { return dev_; }
private:
	latero::Tactograph *dev_;
	GeneratorPtr gen_;
	boost::posix_time::time_duration period_;
    AudioDevicePtr audioDevPtr_;
};

} // namespace
