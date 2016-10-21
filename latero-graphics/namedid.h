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

#ifndef LATERO_GRAPHICS_NAMED_ID
#define LATERO_GRAPHICS_NAMED_ID

#include <string>
#include <vector>
#include <iostream>

namespace latero {
namespace graphics { 

// This class provides basic support for the association between a numerical ID and a 
// label. This is useful in situations where a numerical ID should be used internally
// to speed up operations but a string label should be used for interactions with users
// (e.g. in a GUI or XML file). 
//
// TODO: This implementation needs work.
//

class NamedID
{
public:
	NamedID() : id(0), label("") {}
	NamedID(int id_, std::string label_) : id(id_), label(label_) {}
	bool operator==(const NamedID &other) const { return (other.id == id); }
	bool operator==(int other_id) const { return (other_id == id); }
	bool operator==(std::string other_label) const { return (other_label == label); }
	bool operator!=(const NamedID &other) const { return (other.id != id); }
	bool operator!=(int other_id) const { return (other_id != id); }
	bool operator!=(std::string other_label) const { return (other_label != label); }
	int id;
	std::string label;
};

class NamedIDSet : public std::vector<NamedID>
{
public:
	NamedIDSet() {};
	virtual ~NamedIDSet() {};
	
	bool Append(const NamedID &item)
	{
		NamedID *conflict = Get(item.id);
		if (conflict)
		{
			std::cout << "Error: NamedID("<< item.id <<","<< item.label <<") conflicts with NamedID("<< conflict->id <<","<< conflict->label <<")\n";
			return false;
		}
		push_back(item);
		return true;
	}

	NamedID *Get(std::string label)
	{
		for (unsigned int i=0; i<size(); ++i)
			if ((*this)[i].label == label)
				return &(*this)[i];
		return NULL;
	}

	NamedID *Get(int id)
	{
		for (unsigned int i=0; i<size(); ++i)
			if ((*this)[i].id == id)
				return &(*this)[i];
		return NULL;
	}

};

} // namespace graphics
} // namespace latero

#endif

