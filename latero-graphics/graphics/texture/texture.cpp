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

#include "texture.h"
#include "lineargratingtexture.h"
#include "radialgratingtexture.h"
#include "axialgratingtexture.h"
#include "doublelineargratingtexture.h"
#include "combotexture.h"
#include "motiontexture.h"
#include "vibrotexture.h"
#include "blendtexture.h"
#include "nulltexture.h"
#include "plaintexture.h"
#include "patterntexture.h"
#include "patternops.h"
#include "../../xml.h"
#include "../../units.h"
#include <sys/types.h>
#include <iostream>
#include <sys/stat.h>
#include <gtkmm/box.h>

namespace latero {
namespace graphics { 

const Point Texture::tdCentricPos_default(0,0);
const Point Texture::motionVector_default(50,0);

TexturePtr Texture::Create(const latero::Tactograph *dev, const XMLInputNode &node)
{
	assert(node.GetName() == "Pattern");
	assert(node.GetType() == "Texture");
	std::string type = node.GetSubType();

	printf("name=%s, type=%s, subtype=%s\n", node.GetName().c_str(), node.GetType().c_str(), node.GetSubType().c_str());

	if (type == "LinearGrating")		return LinearGratingTexture::Create(dev,node);
	else if (type == "RadialGrating")	return RadialGratingTexture::Create(dev,node);
	else if (type == "AxialGrating")	return AxialGratingTexture::Create(dev,node);
	else if (type == "Combo")		return ComboTexture::Create(dev,node);
	else if (type == "DoubleLinearGrating")	return DoubleLinearGratingTexture::Create(dev,node);
	else if (type == "Vibration")		return VibroTexture::Create(dev,node);
	else if (type == "Null")		return NullTexture::Create(dev,node);
	else if (type == "Blend")		return BlendTexture::Create(dev,node);
	else if (type == "Motion")		return MotionTexture::Create(dev,node);
	else if (type == "Plain")		return PlainTexture::Create(dev,node);
	else if (type == "Pattern")		return PatternTexture::Create(dev,node);
	else
	{
		printf("Error: Attempted to create a texture of unknown type %s\n", type.c_str());
		return NullTexture::Create(dev);
	}
}


TexturePtr Texture::Create(const latero::Tactograph *dev, std::string xmlFile)
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
				TexturePtr rv = Create(dev,txNode);
				rv->SetXMLFile(xmlFile);
				return rv;
			}
		}
		return NullTexture::Create(dev);
	}
	catch (const std::exception& ex)
  	{
    		std::cout << "Exception caught while loading texture from file " << xmlFile << ": " << ex.what() << std::endl;
		return NullTexture::Create(dev);
  	}
}

Texture::Texture(const latero::Tactograph *dev) :
	Pattern(dev),
    invert_(invert_default),
	amplitude_(amplitude_default),
    vib_(Oscillator::Create(50,1,false)),
	tdCentric_(tdCentric_default),
	tdCentricPos_(tdCentricPos_default),
	motionOverride_(false),
    motionEnable_(motionEnable_default),
	motionVector_(motionVector_default),
	dev_(dev)
{
	AddModifiableChild_(vib_);
}

Texture::~Texture()
{
}

void Texture::Save(std::string xmlFile)
{
	printf("Saving texture to %s...\n", xmlFile.c_str());
	xmlpp::Document document;
	XMLOutputNode root = document.create_root_node("laterographics_xml");
	root.SetPath(Glib::path_get_dirname(xmlFile) + "/");
	root.SetAttribute("type","Texture");
	AppendXML(root);
	document.write_to_file_formatted(xmlFile);
	chmod(xmlFile.c_str(), 0666); // make accessible to others than root
	SetXMLFile(xmlFile);
}

void Texture::AppendXML(XMLOutputNode &root) const
{
	AppendIcon(root);
	AppendAmplitude(root);
	AppendInvert(root);
	AppendVibration(root);
	AppendTDCentric(root);
	AppendMotion(root);
}

void Texture::AppendAmplitude(XMLOutputNode &root) const
{
	root.AddChild("amplitude", GetAmplitude());
}

void Texture::AppendIcon(XMLOutputNode &root) const
{
	if (GetIconFile() != "")
		if (GetIconFile() != GetDefaultIconFile())
			root.AddFilenameChild("icon", GetIconFile());
}

void Texture::AppendInvert(XMLOutputNode &root) const
{
	if (GetInvert()) root.AddChild("invert", GetInvert());
}

void Texture::AppendVibration(XMLOutputNode &root) const
{
	if (GetVibEnable()) vib_->AppendXML(root);
}

void Texture::AppendTDCentric(XMLOutputNode &root) const
{
	if (GetTDCentric()) root.AddChild("td_centric", GetTDCentricPos(), units::mm);
}

void Texture::AppendMotion(XMLOutputNode &root) const
{
	if (GetMotionEnable())
	{
		XMLOutputNode node = root.AddChild("motion");
		node.AddChild("enable", GetMotionEnable());
		node.AddChild("direction", GetMotionDirection(),units::degree);
		node.AddChild("velocity", GetMotionVelocity(),units::mm_per_sec);
	}
}


void Texture::LoadXML(const XMLInputNode &root)
{
	LoadAmplitude(root);
	LoadIcon(root);
	LoadInvert(root);
	LoadVibration(root);
	LoadTDCentric(root);
	LoadMotion(root);
}

void Texture::LoadAmplitude(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("amplitude");
	if (node) SetAmplitude(node.GetFloat());
}

void Texture::LoadIcon(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("icon");
	if (node) SetIconFile(node.GetFilename());
}

void Texture::LoadInvert(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("invert");
	if (node) SetInvert(node.GetBool());
}

void Texture::LoadVibration(const XMLInputNode &root)
{
	vib_->LoadXML(root);
}

void Texture::LoadTDCentric(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("td_centric");	
	if (node)
	{
		SetTDCentric(true);
		SetTDCentricPos(node.GetPoint());
	}
}

void Texture::LoadMotion(const XMLInputNode &root)
{
	SetMotionEnable(false);
	XMLInputNode node = root.GetChild("motion");
	if (node)
	{
		XMLInputNode n = node.GetChild("enable");
		if (n) SetMotionEnable(n.GetBool());

		n = node.GetChild("direction");
		if (n) SetMotionDirection(n.GetFloat());

		n = node.GetChild("velocity");
		if (n) SetMotionVelocity(n.GetFloat());


	}
}

Cairo::RefPtr<Cairo::Pattern> Texture::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	Cairo::RefPtr<Cairo::Pattern> pattern = GetNormDrawingPattern(mmContext,t);
	
	// multiply by amplitude
	pattern = PatternOps::Multiply(mmContext,pattern,GetAmplitude());

	// invert
	if (GetInvert()) pattern = PatternOps::Invert(mmContext,pattern);

	// add vibration
	//if (GetVibEnable()) pattern = PatternOps::Multiply(mmContext,pattern,vib_->GetBlendPattern(mmContext));
	if (GetVibEnable()) pattern = vib_->GetBlendPattern(pattern,mmContext);

	return pattern;
}

Gtk::Widget *Texture::CreateWidget(PatternPtr ptr)
{
	TexturePtr gen = boost::dynamic_pointer_cast<Texture>(ptr);
	if (!gen) return Pattern::CreateWidget(ptr); // default
	return CreateWidget(gen);
}


Gtk::Widget *Texture::CreateWidget(TexturePtr ptr)
{
	return new Gtk::VBox;
}

} // namespace graphics
} // namespace latero



