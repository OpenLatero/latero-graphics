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

#ifndef LATERO_GRAPHICS_XML
#define LATERO_GRAPHICS_XML

#include <libxml++/libxml++.h>
#include "point.h"
#include <string>

namespace latero {
namespace graphics {

class XMLOutputNode
{
public:
	XMLOutputNode(xmlpp::Element *root);
	virtual ~XMLOutputNode() {}

	XMLOutputNode AddChild(std::string name);
	XMLOutputNode AddChild(std::string name, double value, std::string units="");
	XMLOutputNode AddChild(std::string name, bool value);
	XMLOutputNode AddChild(std::string name, int value, std::string units="");
	XMLOutputNode AddChild(std::string name, unsigned int value, std::string units="");
	XMLOutputNode AddChild(std::string name, std::string value);
	XMLOutputNode AddChild(std::string name, const Point &value, std::string units="");
	XMLOutputNode AddChild(std::string name, const char *value);
	XMLOutputNode AddFilenameChild(std::string name, std::string value);

	void SetText(double value, std::string units="");
	void SetText(bool value);
	void SetText(int value, std::string units="");
	void SetText(unsigned int value, std::string units="");
	void SetText(std::string str);
	void SetText(const Point &p, std::string units="");
	void SetText(const char* str);
	void SetTextFilename(std::string str);

	void SetType(std::string content);
	void SetSubType(std::string content);
	void SetAttribute(std::string label, std::string content);
	void SetPath(std::string path) { path_ = path; }
	std::string GetPath() const { return path_; }
protected:
	void SetUnitsAttribute(std::string units);
	xmlpp::Element *element_;
	std::string path_;
};


class XMLInputNode
{
public:
	XMLInputNode(const xmlpp::Node* node);
	virtual ~XMLInputNode() {}

	operator bool() const { return (node_!=NULL); }
	std::vector<XMLInputNode> GetChildren(std::string name) const;
	XMLInputNode GetChild(std::string name) const;

	/** @return text content as a string */
	std::string GetString() const;

	/** @return text content as a float */
	float GetFloat() const;

	/** @return text content as an integer */
	int GetInt() const;

	/** @return text content as a boolean */
	bool GetBool() const;

	/** @return text content as a Point */
	Point GetPoint() const;

	/** @return text content as an absolute filename (using path_ if necessary) */
	std::string GetFilename() const;

	/** @return attribute string for a given label  */
	std::string GetAttribute(std::string label) const;

	/** @return units if specified as a "units" attribute */
	std::string GetUnits() const;

	/** @return type if specified as a "type" attribute */
	std::string GetType() const;

	/** @return type if specified as a "subtype" attribute */
	std::string GetSubType() const;

	/** @return node name */
	std::string GetName() const;
	
	/** @return set path in the case of an XML file */
	void SetPath(std::string path) { path_ = path; }

	/** @return get path (empty if not from an XML file) */
	std::string GetPath() const { return path_; }

protected:
	const xmlpp::Node* node_;
	std::string path_;
};

} // namespace graphics
} // namespace latero

#endif
