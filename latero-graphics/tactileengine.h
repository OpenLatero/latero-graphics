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

#ifndef LATERO_GRAPHICS_TACTILE_ENGINE_H
#define LATERO_GRAPHICS_TACTILE_ENGINE_H

#include <latero/tactograph.h>
#include "engine.h"
#include "generatorfwd.h"

namespace latero {
namespace graphics { 

class TactileEngine : public Engine
{
public:
	TactileEngine(latero::Tactograph *dev, boost::posix_time::time_duration period);
	virtual ~TactileEngine();
	virtual void EntryPoint();
	void SetGenerator(GeneratorPtr gen);
	void ResetGenerator();
	inline const latero::Tactograph *Dev() const { return dev_; }
private:
	latero::Tactograph *dev_;
	GeneratorPtr gen_;
	bool genMod_;
	boost::posix_time::time_duration period_;
};

} // namespace graphics
} // namespace latero

#endif
