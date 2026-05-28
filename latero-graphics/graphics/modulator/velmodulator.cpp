#include "velmodulator.h"
#include "velmodulatorwidget.h"
#include <gtkmm.h>
#include "../../units.h"

namespace latero::graphics {

VelModulator::VelModulator(const latero::Tactograph *dev) :
	Modulator(dev),
	delay_(delay_default),
    stateOut_(true),
    startTime_(boost::posix_time::min_date_time),
    toggleCount_(0)
{
	SetLimit(limit_default);
	SetTransition(transition_default);
}

VelModulator::VelModulator(const latero::Tactograph *dev, const XMLInputNode &node) :
	Modulator(dev),
    delay_(delay_default),
    stateOut_(true),
	startTime_(boost::posix_time::min_date_time),
    toggleCount_(0)
{
	SetLimit(limit_default);
	SetTransition(transition_default);
	LoadXML(node);
}


Gtk::Widget *VelModulator::CreateWidget(ModulatorPtr ptr)
{
	VelModulatorPtr gen = boost::dynamic_pointer_cast<VelModulator>(ptr);
	if (!gen) return Modulator::CreateWidget(ptr); // default
	return Gtk::make_managed<VelModulatorWidget>(gen);
} 

double VelModulator::GetValue_(const ActuatorState &state)
{
	double v = 1.0 - (state.velMag - limit_) / transition_;
	double rv = fmin(fmax(v,0.0),1.0);

	if (state.mode.StateChangeAllowed())
	{
		if (rv==0)
		{
			// we're out of the low-velocity zone
			stateOut_ = true;
		}
		else 
		{
			if (stateOut_)
			{
				// we're stepping inside...
				startTime_ = state.GetTimeElapsed();
				stateOut_ = false;
			}
		}
	}

	if ((rv!=0) && !(state.mode.IgnoreState()))
	{
		// how long have we been inside?
		double msElapsed = (state.GetTimeElapsed() - startTime_).total_microseconds()/1000.0;

		// ramp up over the last half of the delay (?)
		double v = (msElapsed - delay_/2) / (delay_/2);
		rv *= fmax(0,fmin(1,v));
	}
    
    // todo: not sure this can be trusted
    if ((lastValue_==0) && (rv>0))
        toggleCount_++;
    else if ((lastValue_>0) && (rv==0))
        toggleCount_++;
    
    lastValue_ = rv; // keep track of last known value
	return rv;
}


void VelModulator::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Modulator");
	node.SetAttribute("type",GetXMLType());
	node.AddChild("limit", GetLimit(), units::mm_per_sec);
	node.AddChild("transition", GetTransition(), units::mm_per_sec);
	node.AddChild("delay", GetDelay(), units::ms);
}

void VelModulator::LoadXML(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("limit");
	if (node) SetLimit(node.GetFloat());

	node = root.GetChild("transition");
	if (node) SetTransition(node.GetFloat());

	node = root.GetChild("delay");
	if (node) SetDelay(node.GetFloat());
}


} // namespace



