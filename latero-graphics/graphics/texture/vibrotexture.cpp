#include "vibrotexture.h"
#include "vibrotexturewidget.h"
#include "texture.h"
#include "../../xml.h"
#include "../../units.h"
#include "stock.h"

namespace latero::graphics {

VibroTexture::VibroTexture(const latero::Tactograph *dev, double freqHz) : 
	Pattern(dev), Texture(dev), vib_(Oscillator::Create(freqHz,1,true))
{
	SetFreq(freqHz);
}

VibroTexture::VibroTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), Texture(dev), vib_(Oscillator::Create(50,1,true))
{
	SetFreq(50);
	LoadXML(node);
}

Cairo::RefPtr<Cairo::Pattern> VibroTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	return vib_->GetPattern(mmContext);
}

Gtk::Widget *VibroTexture::CreateWidget(TexturePtr ptr)
{
	VibroTexturePtr gen = std::dynamic_pointer_cast<VibroTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return Gtk::make_managed<VibroTextureWidget>(gen);
}

void VibroTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Vibration");
	node.AddChild("freq", GetFreq(), units::hz);
	Texture::AppendXML(node);
}

void VibroTexture::LoadXML(const XMLInputNode &node)
{
	XMLInputNode freqNode = node.GetChild("freq");
	if (freqNode) SetFreq(freqNode.GetFloat());
	Texture::LoadXML(node);
}

std::string VibroTexture::GetDefaultIconFile() const
{
	return tx_icon_vibration;
}

} // namespace
