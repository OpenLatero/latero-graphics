#include "audioengine.h"
#include <stdio.h>
#include <iostream>
#include "mutex.h"
#include "generator.h"
#include "audiodevice.h"

namespace latero::graphics {

AudioEngine::AudioEngine(latero::Tactograph *dev, boost::posix_time::time_duration period) :
	dev_(dev), period_(period)
{
    audioDevPtr_ = AudioDevice::Create();
}

AudioEngine::~AudioEngine()
{
}

void AudioEngine::EntryPoint()
{
	std::cout << "Opening audiere device...\n";
	LATERO_GRAPHICS_LOCK;
	audioDevPtr_->Open();

	LATERO_GRAPHICS_UNLOCK;
	if (audioDevPtr_->IsOpen())
		std::cout << "audiere opened successfully\n";
	else
		std::cout << "audiere could not be opened\n";
    
	while(1)
	{
		boost::this_thread::sleep(period_);
        LATERO_GRAPHICS_LOCK;
		GeneratorPtr gen = gen_;
		LATERO_GRAPHICS_UNLOCK;
		if (gen) gen->PlayAudio(audioDevPtr_);
	}
}

void AudioEngine::SetGenerator(GeneratorPtr gen)
{
	LATERO_GRAPHICS_GUARD;
	gen_ = gen;
}

void AudioEngine::ResetGenerator()
{
	LATERO_GRAPHICS_GUARD;
	gen_.reset();
}

} // namespace
