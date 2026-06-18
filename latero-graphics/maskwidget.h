#pragma once

#include <gtkmm.h>
#include "mask.h"
#include "generator.h"
#include <latero/tactograph.h>

namespace latero::graphics {

class MaskWidgetCtrl 
{
public:
	MaskWidgetCtrl(MaskPtr peer) : disableSignals_(false), peer_(peer) {}
	virtual ~MaskWidgetCtrl() {}
	void SynchFromPeer()
	{
		bool v = disableSignals_;
		disableSignals_ = true;
		SynchFromPeer_();
		disableSignals_ = v;
	}
	virtual void SynchFromPeer_() {}
protected:
	bool disableSignals_;
    MaskPtr peer_;
};

class MaskWidget : public Gtk::Box
{
public:
	MaskWidget(MaskPtr peer, const latero::Tactograph *dev);
	virtual ~MaskWidget() {};
protected:
	void SynchFromPeer();
	std::vector<MaskWidgetCtrl*> ctrls_;
};

} // namespace

