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

#include <sys/stat.h>
#include <gtkmm/box.h>
#include "pattern.h"
#include "circle.h"
#include "line.h"
#include "polygon.h"
#include "dots.h"
#include "image.h"
#include "group.h"
#include "audiopattern.h"
#include "toggle.h"
#include "texture/texture.h"
#include <boost/thread.hpp>

namespace latero {
namespace graphics { 

Pattern::Pattern(const latero::Tactograph *dev) 
	: visible_(true), dev_(dev)
{
}

PatternPtr Pattern::Create(const latero::Tactograph *dev, const XMLInputNode &node)
{
	if (node.GetName() != "Pattern")
		return PatternPtr();

	std::string type = node.GetType();	
	if (type == "Circle")		return Circle::Create(dev, node);
	else if (type == "Line")	return Line::Create(dev, node);
	else if (type == "Polygon")	return Polygon::Create(dev, node);
	else if (type == "Dots")	return Dots::Create(dev, node);
	else if (type == "Dot")		return Dot::Create(dev, node);
	else if (type == "Image")	return Image::Create(dev, node);
	else if (type == "Group")	return Group::Create(dev, node);
	else if (type == "Texture")	return Texture::Create(dev, node);
	else if (type == "Audio")	return AudioPattern::Create(dev, node);
	else if (type == "Toggle")	return Toggle::Create(dev, node);
	else
	{
		printf("Error while attempting to create Pattern from XML file: invalid type '%s'\n", type.c_str());
		return PatternPtr();
	}
}

PatternPtr Pattern::Create(const latero::Tactograph *dev, std::string xmlFile)
{
	try
	{
		xmlpp::DomParser parser;
		parser.set_substitute_entities();
		parser.parse_file(xmlFile);
		if (parser)
		{
			XMLInputNode root = parser.get_document()->get_root_node();
			root.SetPath(Glib::path_get_dirname(xmlFile) + "/");
			XMLInputNode txNode = root.GetChild("Pattern");
			if (txNode)
			{
				PatternPtr rv = Create(dev,txNode);
				rv->SetXMLFile(xmlFile);
				return rv;
			}
		}
		return PatternPtr();
	}
	catch (const std::exception& ex)
  	{
    		std::cout << "Exception caught while loading Pattern from file " << xmlFile << ": " << ex.what() << std::endl;
		return PatternPtr();
  	}
}


Glib::RefPtr<Gdk::Pixbuf> Pattern::GetVisualization(uint w, boost::posix_time::time_duration t, VizMode mode, double velMag, double velDir)
{
	if (mode == viz_abstract)
	{
		uint h = fmax(1,w * Dev()->GetSurfaceHeight()/Dev()->GetSurfaceWidth());
		assert(h>0);

		Glib::RefPtr<Gdk::Pixbuf> img = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, w, h);
		Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(
			(unsigned char*)img->get_pixels(),
			Cairo::FORMAT_ARGB32,
			img->get_width(), img->get_height(), img->get_rowstride());
		Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
		cr->scale(w/Dev()->GetSurfaceWidth(), h/Dev()->GetSurfaceHeight()); // scale to mm

		// clear to white
		cr->set_source_rgb(1.0, 1.0, 1.0);
		cr->paint();
	
		cr->set_source(GetDrawingPattern(cr,t));
		cr->paint();
		return img;
	}
	else
	{
		double ws_x = Dev()->GetSurfaceWidth();
		double ws_y = Dev()->GetSurfaceHeight();
		uint h = w * ws_y/ws_x;

		Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, w, h);

		ActuatorState state;
		if (mode == viz_deflection)
			state.mode = RenderMode(true,false,false);
 		else if (mode == viz_deflection_and_vibration)
			state.mode = RenderMode(true,true,false);
		else // if (mode == viz_shadow)
			state.mode = RenderMode(true,false,false);
		state.i = state.j = 0; 
		state.SetTimeElapsed(t);
		state.velMag = velMag;
		state.velDir = velDir;

		uint rowstride = buf->get_rowstride();
		uint n_channels = buf->get_n_channels();
		guint8* pixels = buf->get_pixels();

		assert(n_channels == 3);

		for (unsigned int pix_y=0; pix_y<h; ++pix_y)
		{
			LATERO_GRAPHICS_LOCK;
			for (unsigned int pix_x=0; pix_x<w; ++pix_x)
			{
				float x = (float)pix_x * (float)ws_x / (float)w;
				float y = (float)pix_y * (float)ws_y / (float)h;
				state.pos = Point(x,y);

				// @todo lock while rendering
				float v = (mode == viz_shadow) ? RenderShadow_(state,true) : Render_(state,true);
				v = (-v+1)*255.0; // we want rest (0) to be white

				guint8* basePixel = pixels + pix_y*rowstride + pix_x*n_channels;
				for (uint n=0; n<3; ++n)
					basePixel[n] = (unsigned char)v;
			}
			LATERO_GRAPHICS_UNLOCK;
			boost::this_thread::yield();
		}
		return buf;
	}
}

void Pattern::SaveToFile(std::string xmlFile)
{
	xmlpp::Document document;
	XMLOutputNode root = document.create_root_node("laterographics_xml");
	root.SetPath(Glib::path_get_dirname(xmlFile) + "/");
	root.SetAttribute("type","Pattern");
	AppendXML(root);
	document.write_to_file_formatted(xmlFile);
	chmod(xmlFile.c_str(), 0666); // make accessible to others than root
	SetXMLFile(xmlFile);
}

double Pattern::Render_(const ActuatorState &state, bool ignoreVisibility)
{
	if (!GetVisible_() && !ignoreVisibility) return 0;
	return DoRender_(state);
}

double Pattern::RenderShadow_(const ActuatorState &state, bool ignoreVisibility)
{
	if (!GetVisible_() && !ignoreVisibility) return 0;
	return DoRenderShadow_(state);
}

bool Pattern::UpdateVisible_(const State *state)
{ 
	SetVisible_(true); 
	return true; 
}

void Pattern::ClearVisible_()
{ 
	SetVisible_(true); 
	/* note: remove to see effect in simulator */ 
}

Gtk::Widget *Pattern::CreateWidget(PatternPtr ptr)
{ 
	return new Gtk::VBox; 
}

} // namespace graphics
} // namespace latero
