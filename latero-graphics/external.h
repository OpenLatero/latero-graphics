#pragma once

#include <sstream>
#include <iostream>

namespace latero::graphics {

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

} // namespace

