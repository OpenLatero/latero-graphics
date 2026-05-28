#include "gratingmodulator.h"
#include <math.h>
#include <iostream>
#include "units.h"

namespace latero::graphics {

const GratingModulator::Mode GratingModulator::mode_slope(0,"slope");
const GratingModulator::Mode GratingModulator::mode_peak(1,"peak");

GratingModulator::GratingModulator(std::string units) :
	units_(units),
	enable_(false),
	length_(113),    
	factor_(5),
	pos_(0),
	mode_(mode_slope)
{
	modeSet_.Append(mode_slope);
	modeSet_.Append(mode_peak);
}

void GratingModulator::AppendXML(XMLOutputNode &root) const
{
	if (GetEnable())
	{
		XMLOutputNode node = root.AddChild("PitchModulator");
		node.SetAttribute("type", GetMode().label);
		node.AddChild("enable", GetEnable());
		node.AddChild("factor", GetFactor());
		node.AddChild("length", GetLength());
		node.AddChild("position", GetPosition());
	}
}

void GratingModulator::LoadXML(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("PitchModulator");
	if (!node)
	{	
		SetEnable(false);
		return;
	}

	std::string mode = node.GetType();
	//std::cout << mode;
	SetMode(mode);

	XMLInputNode n = node.GetChild("enable");
	if (n) SetEnable(n.GetBool());

	n = node.GetChild("factor");
	if (n) SetFactor(n.GetFloat());

	n = node.GetChild("length");
	if (n) SetLength(n.GetFloat());

	n = node.GetChild("position");
	if (n) SetPosition(n.GetFloat());
}

double GratingModulator::ModulatePosition_(double pos)
{
	if (!enable_) return pos;

	if (mode_ == mode_peak)
	{
		double a = pos_;
		double K = factor_;
		double L = length_;
		if (pos > a-L)
		{
			if (pos<a)
				pos = pos + (K-1)*(pos+L-a)*(pos+L-a)/(2*L);
			else if (pos<a+L)
				pos = K*pos - (K-1)*((pos-a)*(pos-a) - L*L)/(2*L) + (1-K)*a;
			else
				pos = pos + (K-1)*L;
		}
	}
	else // mode_slope
	{
		double a = pos_;
		double k = factor_;
		double L = length_;
		if (pos>a)
		{
			if (pos<a+L)
				pos = pos + (k-1)*(pos-a)*(pos-a)/(2*L);
			else
				pos = k*pos + (1-k)*(a+0.5*L);
		}
	}
	return pos;
}

} // namespace
