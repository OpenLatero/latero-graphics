#include "canvaswidget.h"
#include "group.h"
#include "canvas.h"

namespace latero::graphics {

CanvasWidget::CanvasWidget(CanvasPtr peer) :
	Gtk::Box(Gtk::Orientation::VERTICAL)
{
	GroupPtr p = peer->GetGroup();
	append(*p->CreateWidget(p));
}

} // namespace

