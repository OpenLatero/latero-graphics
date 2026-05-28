#pragma once

#include "pattern.h"
#include "../maskfwd.h"
#include "audiopatternfwd.h"
#include "../audiodevice.h"

namespace latero::graphics {

class Sound
{
public:
	Sound(const Point &surfaceSize);
	void SetFile(std::string file);
	std::string GetFile();
	void Play(AudioDevicePtr dev);
	MaskPtr mask_;

protected:
	void UpdateStream();

	AudioDevicePtr dev_;
#ifdef ENABLE_AUDIERE
	audiere::OutputStreamPtr stream_;
#endif
    
    std::string file_;
};

class AudioPattern : public Pattern
{
public:
	static AudioPatternPtr Create(const latero::Tactograph *dev)
	{
		return AudioPatternPtr(new AudioPattern(dev));
	}

	static AudioPatternPtr Create(const latero::Tactograph *dev, const XMLInputNode &node)
	{
		return AudioPatternPtr(new AudioPattern(dev,node));
	}
	
	static AudioPatternPtr Create(const latero::Tactograph *dev, std::string file)
	{
		return AudioPatternPtr(new AudioPattern(dev,file));
	}

	virtual ~AudioPattern();
	virtual void Render_(const State *state, latero::BiasedImg &frame);
	virtual void AppendXML(XMLOutputNode &root) const;

	virtual void PlayAudio(AudioDevicePtr dev, Point pos);

	virtual std::string GetName() { return "AudioPattern"; }
	virtual Cairo::RefPtr<Cairo::Pattern> GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t);
	virtual double DoRender_(const ActuatorState &state) { return 0; }
	virtual double DoRenderShadow_(const ActuatorState &state) { return 0; }

    // @warning: This class currently doesn't use locks. This function should therefore be called ONLY while the AudioPattern
    // is not used by any other threads, i.e. before it gets inserted into a Generator such as Canvas.
    // @todo: fix this!
    void AddSound(MaskPtr mask, std::string audioFile);

protected:
	std::vector<Sound*> sounds_;
	int prevSound_;

private:
	AudioPattern(const latero::Tactograph *dev);
	AudioPattern(const latero::Tactograph *dev, const XMLInputNode &node);
	AudioPattern(const latero::Tactograph *dev, std::string file);
};

} // namespace

