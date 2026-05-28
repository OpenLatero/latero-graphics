#include "gratingtexture.h"
#include "../../grating.h"
#include "../../xml.h"
#include "../../units.h"

namespace latero::graphics {

GratingTexture::GratingTexture(const latero::Tactograph *dev, const Point &seed, std::string units) :
	Pattern(dev), Texture(dev),
	seed_(seed),
	grating_(Grating::Create(units))
{
	AddModifiableChild(grating_);
}

void GratingTexture::AppendXML(XMLOutputNode &node) const
{
	node.AddChild("seed", GetSeed(), units::mm);
	grating_->AppendXML(node);
	Texture::AppendXML(node);
}

void GratingTexture::LoadXML(const XMLInputNode &root)
{
	XMLInputNode node = root.GetChild("seed");
	if (node) SetSeed(node.GetPoint());
	grating_->LoadXML(root);
	Texture::LoadXML(root);
}

Cairo::RefPtr<Cairo::Pattern> GratingTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	return GetGratingDrawingPattern(mmContext,t);
}

} // namespace


