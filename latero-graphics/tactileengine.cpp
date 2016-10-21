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

#include "velocityestimator.h"
#include "tactileengine.h"
#include <iostream>
#include "generator.h"
#include "units.h"
#include "buttonevent.h"

namespace latero {
namespace graphics { 

TactileEngine::TactileEngine(latero::Tactograph *dev, boost::posix_time::time_duration period) :
		Engine(true), dev_(dev), period_(period)
{
}

TactileEngine::~TactileEngine()
{
}

void TactileEngine::EntryPoint()
{
	std::cout << "Refresh rate: " << 1E6/period_.total_microseconds() << " Hz (" << period_.total_microseconds()/1000.0 << " ms)\n";

	//long nb_ticks = 0;
	latero::RangeImg frame(dev_->GetFrameSizeX(), dev_->GetFrameSizeY());
	latero::BiasedImg biasedFrame(dev_->GetFrameSizeX(), dev_->GetFrameSizeY());
	boost::posix_time::ptime lastUpdate = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::ptime start = lastUpdate;
	State state(dev_);
	VelocityEstimator velEstimator;
    ButtonEvent buttonEvent;

	try {
	while (1)
	{
		boost::posix_time::ptime cycleStart = boost::posix_time::microsec_clock::universal_time();

		LATERO_GRAPHICS_LOCK;
		state.tick++;
        
        // if emulated, update position
        /*
        if (dev_->IsEmulated())
        {
            latero::Point emPos;
            emPos.x = ((state.tick%10000)/10000.0) * dev_->GetSurfaceWidth();
            emPos.y = dev_->GetSurfaceHeight() / 2.0;
            dev_->SetEmulatedState(emPos, 0);
        }
        */
        
		boost::posix_time::ptime time = boost::posix_time::microsec_clock::universal_time();
		state.SetTimeElapsed(time - start);
		if (gen_ && GetEnabled()) // minimize work when there is no generator,,,
		{
			if (genMod_ || (gen_->GetLastModified() > lastUpdate))
			{
				genMod_ = false;
				lastUpdate = time;
				dev_->BeginFade();
				std::cout << "fading...\n";
			}

			dev_->ReadCarrierState(state.pos, state.angle);
			velEstimator.Insert(state.pos, state.GetTimeElapsed());
			state.velMag = velEstimator.GetMagnitude();
			state.velDir = velEstimator.GetDirection();
			state.actuatorPositions = dev_->GetActuatorPositions(state.pos, state.angle);
            
            // TODO: figure out a cleaner way to share this event...
            dev_->GetButton0(buttonEvent.button0_up_event, buttonEvent.button0_down_event);
            dev_->GetButton1(buttonEvent.button1_up_event, buttonEvent.button1_down_event);
            if (buttonEvent.button0_up_event || buttonEvent.button0_down_event || buttonEvent.button1_up_event || buttonEvent.button1_down_event)
                gen_->OnButtonEvent_(buttonEvent);
 
			gen_->Render_(&state, biasedFrame);

			LATERO_GRAPHICS_UNLOCK;

			/**
		 	* This operation involves write delays during which other realtime tasks should 
			 * be allowed to run. It is therefore essential NOT to lock the library during
		 	* that time so that the GUI task can use that free time.
		 	*/
			biasedFrame.ConvertToRange(frame); // @todo reimplement WriteFrame to handle BiasedImg efficiently
			dev_->WriteFrame(frame);
			LATERO_GRAPHICS_LOCK;

			//if (!(state.tick%1000))
			//	printf("velocity: %f angle %f\n", velEstimator.GetMagnitude(), units::RadToDegree(velEstimator.GetDirection()));
		}
		LATERO_GRAPHICS_UNLOCK;

		boost::posix_time::ptime cycleEnd = boost::posix_time::microsec_clock::universal_time();
		boost::this_thread::sleep(period_ - (cycleEnd-cycleStart));
	}
	}
	catch (boost::thread_interrupted &e)
	{
		boost::posix_time::time_duration duration = boost::posix_time::microsec_clock::universal_time() - start;
		printf("effective rate: %f Hz (%ld ticks / %f minutes)\n",
				(double)state.tick / (double)duration.total_seconds(),
				state.tick, duration.total_seconds()/60.0);
	}
}

void TactileEngine::SetGenerator(GeneratorPtr gen)
{
	LATERO_GRAPHICS_GUARD;
	gen_ = gen;
	genMod_ = true;
}

void TactileEngine::ResetGenerator()
{
	LATERO_GRAPHICS_GUARD;
	gen_.reset();
	genMod_ = true;
}

} // namespace graphics
} // namespace latero

