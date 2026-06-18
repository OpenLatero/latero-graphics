#pragma once

#include <memory>
#include "../../modifiable.h"
#include <latero/tactograph.h>
#include "../../xml.h"
#include <gtkmm.h>
#include "../../state.h"

namespace latero::graphics {

typedef std::shared_ptr<class Modulator> ModulatorPtr;

/**
 * A modulator takes as input a number of variables (typically sensor readings) and returns
 * a scalar value to be used as a modulator.
 */
class Modulator : public Modifiable
{
public:
	static ModulatorPtr Create(const latero::Tactograph *dev, const XMLInputNode &node);
	virtual void AppendXML(XMLOutputNode &root) const = 0;
	virtual ~Modulator() {}
	virtual Gtk::Widget *CreateWidget(ModulatorPtr ptr);
	const latero::Tactograph *Dev() { return dev_; }
	virtual double GetValue_(const ActuatorState &state) = 0;
	virtual Glib::RefPtr<Gdk::Pixbuf> GetIllustration(uint size, double maxVel=100);
protected:
	Modulator(const latero::Tactograph *dev) : dev_(dev) {}
	const latero::Tactograph *dev_; 
};

} // namespace

