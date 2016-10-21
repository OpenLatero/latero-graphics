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

#include "togglewidget.h"
#include <iostream>
#include "toggle.h"
#include "../pointwidget.h"
#include "dotpattern.h"
#include <gtkmm/frame.h>
#include "../gtk/numwidget.h"
#include "patternpreview.h"

namespace latero {
namespace graphics { 

ToggleWidget::ToggleWidget(TogglePtr peer)
{
	PatternPtr p1 = peer->GetFirstPattern();
	PatternPtr p2 = peer->GetSecondPattern();
	if (p1) append_page(*manage(p1->CreateWidget(p1)), "first");
	if (p2) append_page(*manage(p2->CreateWidget(p2)), "second");
}


} // namespace graphics
} // namespace latero

