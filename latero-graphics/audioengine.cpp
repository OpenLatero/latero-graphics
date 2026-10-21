#include "audioengine.h"
#include <stdio.h>
#include <iostream>
#include "mutex.h"
#include "generator.h"
#include "audiodevice.h"

namespace latero::graphics {

AudioEngine::AudioEngine(latero::Tactograph *dev, double updateRate) :
	dev_(dev), period_(std::chrono::microseconds((long long)(1e6 / updateRate)))
{
    audioDevPtr_ = AudioDevice::Create();
}

AudioEngine::~AudioEngine()
{
}

void AudioEngine::EntryPoint()
{
#ifdef ENABLE_AUDIERE
	std::cout << "Opening audiere device...\n";
#endif
	LATERO_GRAPHICS_LOCK;
	audioDevPtr_->Open();

	LATERO_GRAPHICS_UNLOCK;
#ifdef ENABLE_AUDIERE
	if (audioDevPtr_->IsOpen())
		std::cout << "audiere opened successfully\n";
	else
		std::cout << "audiere could not be opened\n";
#else
	std::cout << "Sound is disabled in this build of latero-graphics: audio patterns are silent.\n";
#endif
    
	auto deadline = std::chrono::steady_clock::now();
	while (!ShouldStop())
	{
		deadline += period_;
		SleepUntil(deadline);
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
