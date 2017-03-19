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

#include "audioengine.h"
#include <stdio.h>
#include <iostream>
#include "mutex.h"
#include "generator.h"
#include "audiodevice.h"

namespace latero {
namespace graphics { 

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

} // namespace graphics
} // namespace latero
