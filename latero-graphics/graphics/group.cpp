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
#include "group.h"
#include "groupwidget.h"
#include "texture/patternops.h"
#include "modulator/modulator.h"
#include "modulator/dirmodulator.h"


namespace latero {
namespace graphics { 

const Group::Operation Group::op_multiply(0,"multiply");
const Group::Operation Group::op_add(1,"add");
const Group::Operation Group::op_max(2,"max");
const Group::Operation Group::op_blend(3,"blend");
const Group::Operation Group::op_over(4,"overwrite");
const Group::Operation Group::op_reactive(5,"reactive");

Group::Group(const latero::Tactograph *dev, const XMLInputNode &node) :
	Pattern(dev), op_(op_max), illMode_(ill_mode_mix)
{
	opSet_.Append(op_multiply);
	opSet_.Append(op_add);
	opSet_.Append(op_max);
	opSet_.Append(op_blend);
	opSet_.Append(op_over);
	opSet_.Append(op_reactive);

	LoadXML(node);
}


Group::Group(const latero::Tactograph *dev) :
	Pattern(dev), op_(op_max), illMode_(ill_mode_mix)
{
	opSet_.Append(op_multiply);
	opSet_.Append(op_add);
	opSet_.Append(op_max);
	opSet_.Append(op_blend);
	opSet_.Append(op_over);
	opSet_.Append(op_reactive);
}

Cairo::RefPtr<Cairo::Pattern> Group::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	// TODO: shares a lot with ComboTexture!

	std::vector<PatternPtr> objects = GetPatterns();
	Group::Operation op = GetOperation();

	mmContext->push_group();
	if (!objects.size()) return mmContext->pop_group();

	Cairo::RefPtr<Cairo::Pattern> pattern = objects[0]->GetDrawingPattern(mmContext,t);
	if (op == op_add)
	{
		for (unsigned int i=1; i<objects.size(); ++i)
			pattern = PatternOps::Add(mmContext, pattern, objects[i]->GetDrawingPattern(mmContext,t));
	}
	else if (op == op_multiply)
	{
		for (unsigned int i=1; i<objects.size(); ++i)
			pattern = PatternOps::Multiply(mmContext, pattern, objects[i]->GetDrawingPattern(mmContext,t));
	}
	else if (op == op_max)
	{
		for (unsigned int i=1; i<objects.size(); ++i)
			pattern = PatternOps::Max(mmContext, pattern, objects[i]->GetDrawingPattern(mmContext,t));
	}
	else if (op == op_blend)
	{
		if (objects.size()>=3)
		{
			Cairo::RefPtr<Cairo::Pattern> a = objects[0]->GetDrawingPattern(mmContext,t);
			Cairo::RefPtr<Cairo::Pattern> b = objects[1]->GetDrawingPattern(mmContext,t);
			Cairo::RefPtr<Cairo::Pattern> k = objects[2]->GetDrawingPattern(mmContext,t);
			Cairo::RefPtr<Cairo::Pattern> ka = PatternOps::Multiply(mmContext,k,a);
			Cairo::RefPtr<Cairo::Pattern> kinv = PatternOps::Invert(mmContext,k);
			Cairo::RefPtr<Cairo::Pattern> kinvb = PatternOps::Multiply(mmContext,kinv,b);
			pattern = PatternOps::Add(mmContext,ka,kinvb);
		}
	}
	else if (op == op_over)
	{
		// TODO: can't get shadow as a pattern yet...
		for (unsigned int i=1; i<objects.size(); ++i)
			pattern = PatternOps::Max(mmContext, pattern, objects[i]->GetDrawingPattern(mmContext,t));
	}
	else if (op == op_reactive)
	{
		ModulatorPtr mod = GetReactiveMod();
		if ((objects.size()>=2)&&(mod))
		{
			ill_mode mode = GetIllMode();
			if (mode == ill_mode_first)
				pattern = objects[0]->GetDrawingPattern(mmContext,t);
			else if  (mode == ill_mode_second)
				pattern = objects[1]->GetDrawingPattern(mmContext,t);
			else
			{
				mmContext->push_group();

				//  top-left half
				mmContext->push_group();
				mmContext->set_source_rgba(1,1,1,1);
				mmContext->rectangle(0,0, Dev()->GetSurfaceWidth()/2, Dev()->GetSurfaceHeight());
				mmContext->fill();
				Cairo::RefPtr<Cairo::Pattern> topleft = mmContext->pop_group();
	
				// bottom-right half
				mmContext->push_group();
				mmContext->set_source_rgba(1,1,1,1);
				mmContext->rectangle(Dev()->GetSurfaceWidth()/2,0, Dev()->GetSurfaceWidth()/2, Dev()->GetSurfaceHeight());
				mmContext->fill();
				Cairo::RefPtr<Cairo::Pattern> bottomright = mmContext->pop_group();

				Cairo::RefPtr<Cairo::Pattern> a = objects[0]->GetDrawingPattern(mmContext,t);
				Cairo::RefPtr<Cairo::Pattern> b = objects[1]->GetDrawingPattern(mmContext,t);

				mmContext->set_source(a);
				mmContext->mask(topleft);

				mmContext->set_source(b);
				mmContext->mask(bottomright);

				int h = 1000;
				int w = h*Dev()->GetSurfaceWidth()/Dev()->GetSurfaceHeight();
				int plotSize = 0.6*h;

				Glib::RefPtr<Gdk::Pixbuf> velplot = mod->GetIllustration(plotSize);
				mmContext->save();
				mmContext->scale(Dev()->GetSurfaceWidth()/w,Dev()->GetSurfaceHeight()/h);

				int innerw = 30; //14	
				int outerw = 50; //22
				mmContext->rectangle(outerw/2,outerw/2, w/2 - outerw, h - outerw);
				mmContext->set_line_width(outerw);
				mmContext->set_source_rgba(0,0,0,1.0);
				mmContext->stroke_preserve();
				mmContext->set_line_width(innerw);
				mmContext->set_source_rgba(1,1,1,1.0);
				mmContext->stroke();
	
				mmContext->rectangle(w/2 + outerw/2, outerw/2, w/2 - outerw, h - outerw);
				mmContext->set_line_width(outerw);
				mmContext->set_source_rgba(0,0,0,1.0);
				mmContext->stroke_preserve();
				mmContext->set_line_width(innerw);
				mmContext->set_source_rgba(0,0,0,1.0);
				mmContext->stroke();

				int arcw = outerw - innerw;
				mmContext->arc(w/2, h/2, 0.5*plotSize + arcw/2, 0, 2*M_PI);
				mmContext->set_line_width(arcw);
				mmContext->set_source_rgba(0.5,0.5,0.5,1.0);
				mmContext->fill();

				Gdk::Cairo::set_source_pixbuf(mmContext,velplot,w/2 - plotSize/2, h/2 - plotSize/2);
				mmContext->paint();
				mmContext->restore();

				pattern = mmContext->pop_group();
			}
		}
	}

	if (pattern)
	{
		mmContext->set_source(pattern);
		mmContext->paint();
	}
	return mmContext->pop_group();
}

void Group::PlayAudio(AudioDevicePtr dev, Point pos)
{
	// TODO: unsafe... should be locked!
	for (unsigned int i=0; i<objects_.size(); ++i)
		objects_[i]->PlayAudio(dev,pos);
}

bool Group::OnKeyPress(GdkEventKey* event)
{
	printf("Group::OnKeyPress\n");
	// TODO: unsafe... should be locked!
	bool rv = false;
	for (unsigned int i=0; i<objects_.size(); ++i)
		rv |= objects_[i]->OnKeyPress(event);
	return rv;
}

void Group::OnButtonEvent_(const ButtonEvent &event)   
{
    for (unsigned int i=0; i<objects_.size(); ++i)
        objects_[i]->OnButtonEvent_(event);
}    


double Group::DoRender_(const ActuatorState &state)
{
	if (objects_.size()<=0) return 0;

	double rv = 0;
	if (op_ == op_multiply)
	{
		rv = objects_[0]->Render_(state);
		for (unsigned int i=1; i<objects_.size(); ++i)
			rv *= objects_[i]->Render_(state);
	}
	else if (op_ == op_add)
	{
		rv = objects_[0]->Render_(state);
		for (unsigned int i=1; i<objects_.size(); ++i)
			rv += objects_[i]->Render_(state);
	}
	else if (op_ == op_max)
	{
		rv = objects_[0]->Render_(state);
		for (unsigned int i=1; i<objects_.size(); ++i)
			rv = fmax(rv, objects_[i]->Render_(state));
	}
	else if (op_ == op_over)
	{
		rv = objects_[0]->Render_(state);
		for (unsigned int i=1; i<objects_.size(); ++i)
		{
			double obj_shadow = objects_[i]->RenderShadow_(state);
			double v = objects_[i]->Render_(state);
			rv = v + (1-obj_shadow)*rv;
		}
	}
	else if (op_ == op_blend)
	{
		// TODO: requires 3 textures, interprets the third one as the mask
		if (objects_.size()>=3)
		{
			double a = objects_[0]->Render_(state);
			double b = objects_[1]->Render_(state);
			double k = objects_[2]->Render_(state);
			rv = k*a + (1-k)*b;
		}
	}
	else if (op_ == op_reactive)
	{
		if ((objects_.size()>=2)&&reactiveMod_)
		{
			double a = objects_[0]->Render_(state);
			double b = objects_[1]->Render_(state);
			double k = reactiveMod_->GetValue_(state);
			rv = k*a + (1-k)*b;
		}
	}

	return fmin(1,rv); // saturate rv;
}

double Group::DoRenderShadow_(const ActuatorState &state)
{
	if (objects_.size()<=0) return 0;

	double shadow = 0;
	if (op_ == op_multiply)
	{
		shadow = objects_[0]->RenderShadow_(state);
		for (unsigned int i=1; i<objects_.size(); ++i)
			shadow *= objects_[i]->RenderShadow_(state);
	}
	else if (op_ == op_add)
	{
		shadow = objects_[0]->RenderShadow_(state);
		for (unsigned int i=1; i<objects_.size(); ++i)
			shadow += objects_[i]->RenderShadow_(state);
		shadow = fmin(1,shadow);
	}
	else if ((op_ == op_max) || (op_ == op_over))
	{
		shadow = objects_[0]->RenderShadow_(state);
		for (unsigned int i=1; i<objects_.size(); ++i)
			shadow = fmax(shadow, objects_[i]->RenderShadow_(state));
	}
	else if (op_ == op_blend)
	{
		if (objects_.size()>=3)
		{
			double a = objects_[0]->RenderShadow_(state);
			double b = objects_[1]->RenderShadow_(state);
			double k = objects_[2]->RenderShadow_(state);
			shadow = k*a + (1-k)*b;
		}
	}
	else if (op_ == op_reactive)
	{
		if ((objects_.size()>=2)&&reactiveMod_)
		{
			double a = objects_[0]->RenderShadow_(state);
			double b = objects_[1]->RenderShadow_(state);
			double k = reactiveMod_->GetValue_(state);
			shadow = k*a + (1-k)*b;
		}
	}
	return fmin(1,shadow); // saturate rv;
}

std::string Group::GetName()
{
	return "Composite";
}

std::vector<PatternPtr> Group::GetPatterns() const
{
	LATERO_GRAPHICS_GUARD;
	return objects_;
}

void Group::InsertPattern(PatternPtr obj)
{
	LATERO_GRAPHICS_GUARD;
	objects_.push_back(obj);
	AddModifiableChild_(obj);
	SetLastModified_();
}

void Group::InsertPatternAfter(PatternPtr newPattern, PatternPtr curPattern)
{
	LATERO_GRAPHICS_GUARD;

	std::vector<PatternPtr>::iterator iter;
	for (iter = objects_.begin(); iter!=objects_.end(); iter++)
	{
    		if (*iter == curPattern)
    		{
			iter++;
			if (iter != objects_.end())
				objects_.insert(iter, newPattern);
			else
				objects_.push_back(newPattern);

			AddModifiableChild_(newPattern);
			SetLastModified_();
			return;
		}
	}
	
}

void Group::InsertPatternBefore(PatternPtr newPattern, PatternPtr curPattern)
{
	LATERO_GRAPHICS_GUARD;

	std::vector<PatternPtr>::iterator iter;
	for (iter = objects_.begin(); iter!=objects_.end(); iter++)
	{
    		if (*iter == curPattern)
    		{
			objects_.insert(iter, newPattern);
			AddModifiableChild_(newPattern);
			SetLastModified_();
			return;
		}
	}
	
}

void Group::MoveUpPattern(PatternPtr pattern)
{
	LATERO_GRAPHICS_GUARD;
	
	// find previous pattern
	PatternPtr prev;
	std::vector<PatternPtr>::iterator iter;
	for (iter = objects_.begin(); iter!=objects_.end(); iter++)
	{
    		if (*iter == pattern)
    		{
			if (iter == objects_.begin())
				break;
			else
				prev = *(iter-1);
		}
	}

	if (prev)
	{
		RemovePattern(pattern);
		InsertPatternBefore(pattern,prev);
		SetLastModified_();
	}
}

void Group::MoveDownPattern(PatternPtr pattern)
{
	LATERO_GRAPHICS_GUARD;
	
	// find next pattern
	PatternPtr next;
	std::vector<PatternPtr>::iterator iter;
	for (iter = objects_.begin(); iter!=objects_.end(); iter++)
	{
    		if (*iter == pattern)
    		{
			if ((iter+1) == objects_.end())
				break;
			else
				next = *(iter+1);
		}
	}

	if (next)
	{
		RemovePattern(pattern);
		InsertPatternAfter(pattern,next);
		SetLastModified_();
	}
}


void Group::RemovePattern(PatternPtr obj)
{
	LATERO_GRAPHICS_GUARD;
	RemoveModifiableChild_(obj);
	std::vector<PatternPtr>::iterator iter;
	for (iter = objects_.begin(); iter!=objects_.end(); iter++)
	{
    		if (*iter == obj)
    		{
        		objects_.erase(iter);
        		break;
    		}
	}
	SetLastModified_();
}

void Group::ReplacePattern(PatternPtr oldPattern, PatternPtr newPattern)
{
	LATERO_GRAPHICS_GUARD;
	RemoveModifiableChild_(oldPattern);
	std::vector<PatternPtr>::iterator iter;
	for (iter = objects_.begin(); iter!=objects_.end(); iter++)
	{
    		if (*iter == oldPattern)
    		{
        		*iter = newPattern;
			AddModifiableChild_(newPattern);
        		break;
    		}
	}
	SetLastModified_();
}


void Group::ClearPatterns_()
{
	for (unsigned int i=0; i<objects_.size(); ++i)
		RemoveModifiableChild_(objects_[i]);
	objects_.clear();
}

Gtk::Widget *Group::CreateWidget(PatternPtr ptr)
{
	GroupPtr gen = boost::dynamic_pointer_cast<Group>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return new GroupWidget(gen);
}

void Group::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Group");

	AppendOperation(node);
	AppendPatterns(node);
}

void Group::AppendPatterns(XMLOutputNode &root) const
{
	std::vector<PatternPtr> list = GetPatterns();
	for (uint i=0; i<list.size(); ++i)
		list[i]->AppendXML(root);

}

void Group::AppendOperation(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("operation");
	node.SetType(GetOperation().label);
	if (GetOperation()==op_reactive)
		if (GetReactiveMod())
			GetReactiveMod()->AppendXML(node);
}


void Group::LoadXML(const XMLInputNode &root)
{
	LoadPatterns(root);
	LoadOperation(root);
}

void Group::LoadPatterns(const XMLInputNode &root)
{
	std::vector<XMLInputNode> list = root.GetChildren("Pattern");
	for (unsigned int i=0; i<list.size(); ++i)
		InsertPattern(Pattern::Create(Dev(), list[i]));	
}


void Group::LoadOperation(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("operation");
	if (!node) return;
	std::string type = node.GetType();
	if (type != "")
	{
		SetOperation(type);
		if (op_ == op_reactive)
		{
			XMLInputNode modNode = node.GetChild("Modulator");
			if (modNode) SetReactiveMod(Modulator::Create(Dev(), modNode));
		}
	}
	else
	{
		// TODO: legacy code, remove when all XML files are updated.
		SetOperation(node.GetString());
	}
}


void Group::SetOperation(std::string label)
{
	Operation *op = opSet_.Get(label);
	if (op) SetOperation(*op);
}

void Group::SetOperation(const Operation &op)
{
	LATERO_GRAPHICS_GUARD;
	op_ = op;
	if ((op_ == op_reactive) && (!reactiveMod_))
	{
		reactiveMod_ = DirModulator::Create(Dev());
		AddModifiableChild_(reactiveMod_);
	}
	SetLastModified_();
}

void Group::ReplacePattern(int pos, PatternPtr tx)
{
	LATERO_GRAPHICS_GUARD;
	if (pos>=(int)objects_.size()) return;
	RemoveModifiableChild_(objects_[pos]);
	objects_[pos] = tx;
	AddModifiableChild_(tx);
	SetLastModified_();
}

bool Group::UpdateVisible_(const State *state)
{
	bool rv = false;
	Pattern::UpdateVisible_(state);
	for (unsigned int i=0; i<objects_.size(); ++i)
	{
		bool vis = objects_[i]->UpdateVisible_(state);
		rv = rv || vis;
	}
	SetVisible_(rv);
	return rv;
}

void Group::ClearVisible_()
{
	Pattern::ClearVisible_();
	for (unsigned int i=0; i<objects_.size(); ++i)
		objects_[i]->ClearVisible_();
}

void Group::SetReactiveMod(ModulatorPtr mod)
{
	LATERO_GRAPHICS_GUARD;
	if (reactiveMod_) RemoveModifiableChild_(reactiveMod_);
	reactiveMod_ = mod;
	AddModifiableChild_(mod);
	SetLastModified_();
}

} // namespace graphics
} // namespace latero
