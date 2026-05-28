#include "nulltexture.h"
#include "texture.h"
#include "stock.h"

namespace latero::graphics {

NullTexture::NullTexture(const latero::Tactograph *dev) :
	Pattern(dev), Texture(dev)
{
}

double NullTexture::NormRender_(const ActuatorState &state)
{
	return 0.0;
}

Cairo::RefPtr<Cairo::Pattern> NullTexture::GetNormDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	mmContext->push_group();
	mmContext->set_source_rgba(0,0,0,0);
	mmContext->paint();
	return mmContext->pop_group();
}

Gtk::Widget *NullTexture::CreateWidget(TexturePtr ptr)
{
	return Texture::CreateWidget(ptr); // default
}

void NullTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("Null");
	Texture::AppendXML(node);
}

std::string NullTexture::GetDefaultIconFile() const
{
	return tx_icon_null;
}

} // namespace
