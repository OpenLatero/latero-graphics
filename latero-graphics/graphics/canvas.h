#pragma once

#include <memory>
#include "../positiongen.h"
#include "group.h"
#include "pattern.h"

namespace latero::graphics {

typedef std::shared_ptr<class Canvas> CanvasPtr;

/**
 * The Canvas is a specialization of the PositionGen, and hence indirectly of Generator, which is used for the rendering of
 * tactile graphics. The actual rendering is done by Pattern objects.
 *
 * @ingroup GeneratorGroup 
 * 
 * @todo A Canvas should ideally hold a single arbitrary Pattern instead of a Group. The use of a Group is a quick hack
 * to simply the CanvasWidget so that patterns can easilly be added or removed. This causes minor inefficiencies when
 * a single pattern is displayed and always shows a Group within the list of patterns even if not needed.
 *
 * @todo This library used to contain many different types of Generators. All that remains is Canvas and two abstract classes,
 * Generator and PositionGen. It may be a good idea to eventually collapse all three into a single class for simplicity. The
 * current organization, however, allows for Generators that operate based on principles other than the Canvas and could 
 * therefore be useful for testing other rendering concepts.
 */
class Canvas : public PositionGen
{
public:
	/** creates an empty Canvas */
	static CanvasPtr Create(const latero::Tactograph *dev);

	/** creates and initializes a canvas based on the content of an XML node */
	static CanvasPtr Create(const latero::Tactograph *dev, const XMLInputNode &node);

	/** destructor */
	virtual ~Canvas();

	/** @return pointer to Group contained in the Canvas */
	GroupPtr GetGroup() const;

	// see Generator
	virtual void AppendXML(XMLOutputNode &root) const;

	// see Generator
	virtual latero::graphics::gtk::Animation GetIllustration(uint w, boost::posix_time::time_duration t) const;

	// see Generator
	virtual Gtk::Widget *CreateWidget(GeneratorPtr ptr);

	// see Generator
	virtual void PlayAudio(AudioDevicePtr dev);
    
	// see Generator
	virtual bool OnKeyPress(guint keyval, guint keycode, Gdk::ModifierType state);

    // see Generator
    virtual void OnButtonEvent_(const ButtonEvent &event);
    
protected:
	/** constructor */
	Canvas(const latero::Tactograph *dev);

	/** constructor from XML node */
	Canvas(const latero::Tactograph *dev, const XMLInputNode &node);

	// see PositionGen
	virtual void PreRender_(const State *state);

	// see PositionGen
	virtual double Render_(const ActuatorState &state, bool stateless=false);

	// see PositionGen
	virtual void PostRender_(const State *state);

	/** Group of Patterns displayed in the Canvas */
	GroupPtr objects_;
};

} // namespace

