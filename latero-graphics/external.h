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

#ifndef LATERO_GRAPHICS_EXTERNAL_H
#define LATERO_GRAPHICS_EXTERNAL_H

#include <sstream>
#include <iostream>

namespace latero {
namespace graphics {

class SysCommand
{
public:
	SysCommand(std::string executable) { cmd << executable; }
	~SysCommand() {}

	void Append(std::string arg) { cmd << " " << arg; }

	void Append(const std::vector<std::string> &args)
	{
		for (unsigned int i=0; i<args.size(); ++i)
			Append(args[i]);
	}

	void Run()
	{
		std::cout << cmd.str() << "\n";
		system(cmd.str().c_str());
	}

protected:
	std::stringstream cmd;
};

} // namespace graphics
} // namespace latero

#endif
