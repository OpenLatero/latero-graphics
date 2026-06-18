#pragma once

#include "closedpattern.h"
#include "../gtk/checkframe.h"
#include "texture/textureselectorwidget.h"

namespace latero::graphics {

class ClosedPatternFillTextureWidget : public gtk::CheckFrame
{
public:
	ClosedPatternFillTextureWidget(ClosedPatternPtr peer);
	virtual ~ClosedPatternFillTextureWidget() {};
protected:
	void OnCheck();
	void OnTextureChanged();

	TextureSelectorWidget txWidget_;
	ClosedPatternPtr peer_;
};

} // namespace

