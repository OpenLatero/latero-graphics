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

#include <iostream>
#include <sys/stat.h>
#include <gtkmm/frame.h>
#include "generator.h"
#include "graphics/canvas.h"

namespace latero {
namespace graphics { 

void Generator::SaveToFile(std::string filename) const
{
	xmlpp::Document document;
	XMLOutputNode root = document.create_root_node("laterographics_xml");
	root.SetAttribute("type","Generator");
	root.SetPath(Glib::path_get_dirname(filename) + "/");
	AppendXML(root);
	document.write_to_file_formatted(filename);
	chmod(filename.c_str(), 0666); // make accessible to others than root
}

GeneratorPtr Generator::Create(std::string xmlFile, const latero::Tactograph *dev)
{
	try
	{
		std::string path = Glib::path_get_dirname(xmlFile) + "/";
		//Generator *gen = NULL;
		xmlpp::DomParser parser;
		//parser.set_validate();
		parser.set_substitute_entities();
		parser.parse_file(xmlFile);
		if (parser)
		{
			XMLInputNode root = parser.get_document()->get_root_node();
			root.SetPath(path);
			XMLInputNode genNode = root.GetChild("Generator");
			if (genNode) return Create(genNode, dev);
		}
		return GeneratorPtr();
	}
	catch (const std::exception& ex)
  	{
    		std::cout << "Exception caught while loading generator from file " << xmlFile << ": " << ex.what() << std::endl;
		return GeneratorPtr();
  	}

}

GeneratorPtr Generator::Create(const XMLInputNode &node, const latero::Tactograph *dev)
{
	assert(node.GetName() == "Generator");
	std::string type = node.GetType();

	if (type == "Canvas")
		return Canvas::Create(dev, node);
	else
	{
		printf("Generator::Create: Invalid Type '%s'\n", type.c_str());
		return GeneratorPtr();
	}
}

Generator::Generator(const latero::Tactograph *dev) :
	dev_(dev),    
	latestFrame_(dev->GetFrameSizeX(), dev->GetFrameSizeY()),
    latestState_(dev)
{
	latestFrame_.Set(0);
	assert(dev_);
};

latero::graphics::gtk::Animation Generator::GetIllustration(uint w, boost::posix_time::time_duration t) const
{
	// by default return a white image
	uint h = fmax(1,w * Dev()->GetSurfaceHeight()/Dev()->GetSurfaceWidth());
	assert(h>0);
	Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, w, h);
	buf->fill(0xffffffff);
	latero::graphics::gtk::Animation rv(buf);
	return rv;
}

Gtk::Widget *Generator::CreateWidget(GeneratorPtr ptr)
{ 
	return new Gtk::Frame(); 
};

double Generator::GetDisplayOrientation() const
{
	LATERO_GRAPHICS_GUARD;
	return latestState_.angle;
}

latero::BiasedImg Generator::GetLatestFrame() const
{ 
	LATERO_GRAPHICS_GUARD; return latestFrame_; 
}

void Generator::PlayAudio(AudioDevicePtr dev)
{
	// do nothing by default
};

void Generator::SetLatestState_(const State &state, const latero::BiasedImg &frame)
{
	latestFrame_ = frame;
	latestState_ = state;
}

bool Generator::OnKeyPress(GdkEventKey* event)
{
	return false;
}

void Generator::Render_(const State *state, latero::BiasedImg &frame)
{
	DoRender_(state, frame);
	SetLatestState_(*state, frame);
}



} // namespace graphics
} // namespace latero
