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

#include "canvaswidget.h"
#include "canvas.h"
#include "group.h"

namespace latero {
namespace graphics {

CanvasPtr Canvas::Create(const latero::Tactograph *dev)
{
	return CanvasPtr(new Canvas(dev));
}

CanvasPtr Canvas::Create(const latero::Tactograph *dev, const XMLInputNode &node)
{
	return CanvasPtr(new Canvas(dev, node));
}


Canvas::Canvas(const latero::Tactograph *dev) :
	PositionGen(dev),
	objects_(Group::Create(dev))
{
	AddModifiableChild(objects_);
}

Canvas::Canvas(const latero::Tactograph *dev, const XMLInputNode &node) :
	PositionGen(dev)
{
	XMLInputNode setNode = node.GetChild("Pattern");
	objects_ = Group::Create(dev,setNode);

	AddModifiableChild(objects_);
}

void Canvas::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Generator");
	node.SetAttribute("type","Canvas");
	objects_->AppendXML(node);
}

Canvas::~Canvas()
{
}

latero::graphics::gtk::Animation Canvas::GetIllustration(uint w, boost::posix_time::time_duration t) const
{
	uint h = fmax(1,w * Dev()->GetSurfaceHeight()/Dev()->GetSurfaceWidth());
	Glib::RefPtr<Gdk::Pixbuf> img = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, w, h);
	Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(
		(unsigned char*)img->get_pixels(), Cairo::FORMAT_ARGB32,
		img->get_width(), img->get_height(), img->get_rowstride());
	Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

	// switch to virtual space (works well when shapes are filled)
	cr->scale(w/Dev()->GetSurfaceWidth(), h/Dev()->GetSurfaceHeight());

	// clear to white
	cr->set_source_rgb(1.0, 1.0, 1.0); // bgr
	cr->paint();

	cr->set_source(objects_->GetDrawingPattern(cr, t));
	cr->paint();

	latero::graphics::gtk::Animation rv(img);
	return rv;	
}

void Canvas::PreRender_(const State *state)
{
	objects_->UpdateVisible_(state);
}

double Canvas::Render_(const ActuatorState &state, bool stateless)
{
	return objects_->Render_(state, stateless);
}

void Canvas::PostRender_(const State *state)
{
	objects_->ClearVisible_();
}

Gtk::Widget *Canvas::CreateWidget(GeneratorPtr ptr)
{
	CanvasPtr gen = boost::dynamic_pointer_cast<Canvas>(ptr);
	if (!gen) return Generator::CreateWidget(ptr); // default
	return new CanvasWidget(gen);
}

void Canvas::PlayAudio(audiere::AudioDevicePtr dev)
{
	return GetGroup()->PlayAudio(dev,GetDisplayCenter());
}

bool Canvas::OnKeyPress(GdkEventKey* event)
{
	return GetGroup()->OnKeyPress(event);
}

void Canvas::OnButtonEvent_(const ButtonEvent &event) 
{
    return GetGroup()->OnButtonEvent_(event);
};    
    
GroupPtr Canvas::GetGroup() const
{ 
	LATERO_GRAPHICS_GUARD;
	return objects_; 
}

} // namespace graphics
} // namespace latero
