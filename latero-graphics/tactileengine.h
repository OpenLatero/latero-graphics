#pragma once

#include <latero/tactograph.h>
#include "engine.h"
#include "generatorfwd.h"

namespace latero::graphics {

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

} // namespace

