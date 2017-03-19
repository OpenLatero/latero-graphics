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

#include "pattern.h"
#include "toggle.h"
#include <gdk/gdkkeysyms.h>
#include "togglewidget.h"
#include <iostream>

namespace latero {
namespace graphics { 

Toggle::Toggle(const latero::Tactograph *dev, const XMLInputNode &node) :
	Pattern(dev), toggle_(false), toggleCount_(0)
{
	LoadXML(node);
}

Toggle::Toggle(const latero::Tactograph *dev) :
	Pattern(dev), toggle_(false), toggleCount_(0)
{
}

Toggle::Toggle(const latero::Tactograph *dev, PatternPtr p1, PatternPtr p2) :
	Pattern(dev), toggle_(false), toggleCount_(0)
{
	patterns_[0] = p1;
	patterns_[1] = p2;
	curPattern_ = p1;
}


Cairo::RefPtr<Cairo::Pattern> Toggle::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	PatternPtr pattern = GetCurrentPattern();
	if (pattern)
	{
		return pattern->GetDrawingPattern(mmContext,t);
	}
	else
	{
		mmContext->push_group();
		return mmContext->pop_group();
	}
}

void Toggle::OnButtonEvent_(const ButtonEvent &event)
{
    if (event.button0_down_event || event.button1_down_event)
    {
        // trigger toggle from Tactograph buttons
        DoToggle_();
    }        
}

void Toggle::PlayAudio(AudioDevicePtr dev, Point pos)
{
	PatternPtr pattern = GetCurrentPattern();
	if (pattern) pattern->PlayAudio(dev,pos);
}

double Toggle::DoRender_(const ActuatorState &state)
{    
	if (curPattern_)
		return curPattern_->Render_(state);
	else
		return 0;
}

double Toggle::DoRenderShadow_(const ActuatorState &state)
{
	if (curPattern_)
		return curPattern_->RenderShadow_(state);
	else
		return 0;
}

std::string Toggle::GetName()
{
	return "Toggle";
}

Gtk::Widget *Toggle::CreateWidget(PatternPtr ptr)
{
	TogglePtr gen = boost::dynamic_pointer_cast<Toggle>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new ToggleWidget(gen);
}

void Toggle::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Toggle");

	// todo lock!
	if (patterns_[0]) patterns_[0]->AppendXML(root);
	if (patterns_[1]) patterns_[1]->AppendXML(root);
}

void Toggle::DoToggle_()
{
    toggle_ = !toggle_;
    if (toggle_)
        curPattern_ = patterns_[1];
    else
        curPattern_ = patterns_[0];
    toggleCount_++;
    SetLastModified_();
}
    
void Toggle::DoToggle()
{
	LATERO_GRAPHICS_GUARD;
    DoToggle_();
}

void Toggle::LoadXML(const XMLInputNode &root)
{
	std::vector<XMLInputNode> list = root.GetChildren("Pattern");
	if (list.size() >= 2)
	{
		patterns_[0] = Pattern::Create(Dev(), list[0]);
		patterns_[1] = Pattern::Create(Dev(), list[1]);
		curPattern_ = patterns_[0];
		toggle_ = false;
	}
}

/*
bool Toggle::OnKeyPress(GdkEventKey* event)
{
	if ((event->keyval == GDK_Control_L)||(event->keyval == GDK_space))
    {
		DoToggle();
        return true;
    }
    return false;
}
*/


} // namespace graphics
} // namespace latero
