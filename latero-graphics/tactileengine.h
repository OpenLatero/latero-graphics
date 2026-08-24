#pragma once

#include <chrono>
#include <latero/tactograph.h>
#include "engine.h"
#include "generator.h"

namespace latero::graphics {

class TactileEngine : public Engine
{
public:
	TactileEngine(latero::Tactograph *dev, double frequency);
	virtual ~TactileEngine();
	virtual void EntryPoint();
	void SetGenerator(GeneratorPtr gen);
	void ResetGenerator();
	inline const latero::Tactograph *Dev() const { return dev_; }
private:
	latero::Tactograph *dev_;
	GeneratorPtr gen_;
	bool genMod_;
	const std::chrono::microseconds period_;
};

} // namespace

