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

#include "xml.h"
#include <iostream>
#include "units.h"

#include <stdio.h>
#include <stdlib.h>

/** create object for loading as well */

namespace latero {
namespace graphics {

XMLOutputNode::XMLOutputNode(xmlpp::Element *root) : element_(root)
{
}

XMLOutputNode XMLOutputNode::AddChild(std::string name)
{
	XMLOutputNode rv = element_->add_child_element(name);
	rv.SetPath(path_);
	return rv;
}

XMLOutputNode XMLOutputNode::AddChild(std::string name, double value, std::string units)
{
	XMLOutputNode rv = AddChild(name);
	rv.SetText(value,units);
	return rv;
}

XMLOutputNode XMLOutputNode::AddChild(std::string name, bool value)
{
	XMLOutputNode rv = AddChild(name);
	rv.SetText(value);
	return rv;
}

XMLOutputNode XMLOutputNode::AddChild(std::string name, int value, std::string units)
{
	XMLOutputNode rv = AddChild(name);
	rv.SetText(value,units);
	return rv;
}

XMLOutputNode XMLOutputNode::AddChild(std::string name, unsigned int value, std::string units)
{
	XMLOutputNode rv = AddChild(name);
	rv.SetText(value,units);
	return rv;
}

XMLOutputNode XMLOutputNode::AddChild(std::string name, std::string value)
{
	XMLOutputNode rv = AddChild(name);
	rv.SetText(value);
	return rv;
}

XMLOutputNode XMLOutputNode::AddChild(std::string name, const Point &value, std::string units)
{
	XMLOutputNode rv = AddChild(name);
	rv.SetText(value,units);
	return rv;
}

XMLOutputNode XMLOutputNode::AddChild(std::string name, const char *value)
{
	XMLOutputNode rv = AddChild(name);
	rv.SetText(value);
	return rv;
}

XMLOutputNode XMLOutputNode::AddFilenameChild(std::string name, std::string value)
{
	XMLOutputNode rv = AddChild(name);
	rv.SetTextFilename(value);
	return rv;
}


void XMLOutputNode::SetUnitsAttribute(std::string units)
{
	if (units=="") return;
	if (units == units::degree)
		element_->set_attribute("units","degree");
	else if (units == units::degree_per_sec)
		element_->set_attribute("units","degree/s");
	else
		element_->set_attribute("units",units);
}

void XMLOutputNode::SetText(bool v)
{
	element_->set_first_child_text(v?"yes":"no");
}

void XMLOutputNode::SetText(double value, std::string units)
{
	char buf[255];
	sprintf(buf, "%g", value);
	element_->set_first_child_text(buf);
	SetUnitsAttribute(units);
}

void XMLOutputNode::SetText(int value, std::string units)
{
	char buf[255];
	sprintf(buf, "%d", value);
	element_->set_first_child_text(buf);
	SetUnitsAttribute(units);
}

void XMLOutputNode::SetText(unsigned int value, std::string units)
{
	char buf[255];
	sprintf(buf, "%d", value);
	element_->set_first_child_text(buf);
	SetUnitsAttribute(units);
}

void XMLOutputNode::SetText(std::string str)
{
	element_->set_first_child_text(str.c_str());
}

void XMLOutputNode::SetText(const Point &p, std::string units)
{
	char buf[255];
	sprintf(buf, "%g,%g", p.x, p.y);
	element_->set_first_child_text(buf);
	SetUnitsAttribute(units);
}

void XMLOutputNode::SetTextFilename(std::string file)
{
	// strip away path_ if identical
	std::string::size_type pos = file.find(path_,0);
	if (pos != std::string::npos)
		file.erase(pos, pos + path_.size());
	SetText(file);
}

void XMLOutputNode::SetText(const char *str)
{
	element_->set_first_child_text(str);
}

void XMLOutputNode::SetAttribute(std::string label, std::string content)
{
	element_->set_attribute(label,content);
}

void XMLOutputNode::SetType(std::string content)
{
	SetAttribute("type",  content);
}

void XMLOutputNode::SetSubType(std::string content)
{
	SetAttribute("subtype",  content);
}

XMLInputNode::XMLInputNode(const xmlpp::Node* node)
	: node_(node)
{
}

std::vector<XMLInputNode> XMLInputNode::GetChildren(std::string name) const
{
	std::vector<XMLInputNode> rv;
	if (!node_) return rv;

	xmlpp::Node::const_NodeList list = node_->get_children();
	for(xmlpp::Node::const_NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node* node = *iter;
		if (node->get_children().size())
		{
			if (node->get_name() == name)
			{
				XMLInputNode n = node;
				n.SetPath(GetPath());
				rv.push_back(n);
			}
		}
	}
	return rv;
}

XMLInputNode XMLInputNode::GetChild(std::string name) const
{
	if (!node_)
	{
		XMLInputNode n = NULL;
		n.SetPath(GetPath());
		return n;
	}

	xmlpp::Node::const_NodeList list = node_->get_children();
	for(xmlpp::Node::const_NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node* node = *iter;
		if (node->get_children().size())
		{
			if (node->get_name() == name)
			{
				XMLInputNode n = node;
				n.SetPath(GetPath());
				return n;
			}
		}
	}

	XMLInputNode n = NULL;
	n.SetPath(GetPath());
	return n;
}

std::string XMLInputNode::GetString() const
{
	const xmlpp::Element *el = dynamic_cast<const xmlpp::Element *>(node_);
	if (el)
	{
		const xmlpp::TextNode* text = el->get_first_child_text();
		if (text)
			return text->get_content();
	}
	return "";
}
	
float XMLInputNode::GetFloat() const
{
	return atof(GetString().c_str());
}

int XMLInputNode::GetInt() const
{
	return atoi(GetString().c_str());
}

bool XMLInputNode::GetBool() const
{
	std::string str = GetString();
	return ((str == "yes") || (str == "true"));
}

Point XMLInputNode::GetPoint() const
{
	Point rv;
	sscanf(GetString().c_str(), "%lf,%lf", &rv.x, &rv.y);
	return rv;
}

std::string XMLInputNode::GetUnits() const
{
	std::string rv = GetAttribute("units");
	if (rv == "degree")
		rv = units::degree;
	else if (rv == "degree/s")
		rv = units::degree_per_sec;
	return rv;
}

std::string XMLInputNode::GetType() const 
{
	return GetAttribute("type");
}

std::string XMLInputNode::GetSubType() const 
{
	return GetAttribute("subtype");
}


std::string XMLInputNode::GetAttribute(std::string label) const
{
	const xmlpp::Element* element = dynamic_cast<const xmlpp::Element*>(node_);
	const xmlpp::Attribute *att = element->get_attribute(label);
	return att ? att->get_value() : "";
}

std::string XMLInputNode::GetName() const
{
	if (!node_) return "";
	return node_->get_name();
}

std::string XMLInputNode::GetFilename() const
{
	std::string file = GetString();
	if (file == "") return file;
	if (file[0] == '/')
		return file; // absolute path
	else
		return GetPath() + file; // relative path
}

} // namespace graphics
} // namespace latero

