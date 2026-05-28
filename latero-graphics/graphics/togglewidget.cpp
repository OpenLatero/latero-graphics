#include "togglewidget.h"
#include <iostream>
#include "toggle.h"
#include "../pointwidget.h"
#include "dotpattern.h"
#include "../gtk/numwidget.h"
#include "patternpreview.h"

namespace latero::graphics {

ToggleWidget::ToggleWidget(TogglePtr peer)
{
	PatternPtr p1 = peer->GetFirstPattern();
	PatternPtr p2 = peer->GetSecondPattern();
	if (p1) append_page(*p1->CreateWidget(p1), "first");
	if (p2) append_page(*p2->CreateWidget(p2), "second");
}


} // namespace

