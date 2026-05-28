#include "plaintexture.h"
#include "plaintexturewidget.h"
#include "texture.h"
#include "../../xml.h"
#include "../../units.h"
#include "stock.h"

namespace latero::graphics {

PlainTexture::PlainTexture(const latero::Tactograph *dev) : 
	Pattern(dev), Texture(dev)
{
}

PlainTexture::PlainTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), Texture(dev)
{
	LoadXML(node);
}

Cairo::RefPtr<Cairo::Pattern> PlainTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	mmContext->push_group();
	mmContext->set_source_rgba(0,0,0,1);
	mmContext->paint();
	return mmContext->pop_group();
}

Gtk::Widget *PlainTexture::CreateWidget(TexturePtr ptr)
{
	PlainTexturePtr gen = boost::dynamic_pointer_cast<PlainTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return Gtk::make_managed<PlainTextureWidget>(gen);
}

void PlainTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Plain");
	Texture::AppendXML(node);
}

void PlainTexture::LoadXML(const XMLInputNode &node)
{
	Texture::LoadXML(node);
}

std::string PlainTexture::GetDefaultIconFile() const
{
	return tx_icon_null;
}


} // namespace
