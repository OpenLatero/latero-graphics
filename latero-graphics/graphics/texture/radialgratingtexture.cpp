#include "radialgratingtexture.h"
#include "radialgratingtexturewidget.h"
#include "../../grating.h"
#include "../../xml.h"
#include "stock.h"

namespace latero::graphics {

RadialGratingTexture::RadialGratingTexture(const latero::Tactograph *dev, Point seed, float pitch) : 
	Pattern(dev), GratingTexture(dev,seed,"mm")
{
	GetGrating()->SetVel(default_vel);
	GetGrating()->SetRidgeSize(pitch);
}

RadialGratingTexture::RadialGratingTexture(const latero::Tactograph *dev, const XMLInputNode &node) : 
	Pattern(dev), GratingTexture(dev,Point(0,0),"mm")
{
	GetGrating()->SetVel(default_vel);
	GetGrating()->SetRidgeSize(default_pitch);
	LoadXML(node);
}

RadialGratingTexture::~RadialGratingTexture()
{
}

double RadialGratingTexture::NormRender_(const ActuatorState &state)
{
	Point delta = state.pos - seed_;
	float d = sqrt(delta.x*delta.x + delta.y*delta.y); // todo use Norm()
	return grating_->Render_(d,state.GetTimeElapsed());
}

Cairo::RefPtr<Cairo::Pattern> RadialGratingTexture::GetGratingDrawingPattern(Cairo::RefPtr<Cairo::Context> mmContext, boost::posix_time::time_duration t)
{
	mmContext->push_group();

	Point seed = GetSeed();
	double delta = GetGrating()->GetMinRidgeSize()/10;
	GratingPtr grating = GetGrating(); // TODO: should be cloned

	Point clip1, clip2;
	mmContext->get_clip_extents(clip1.x, clip1.y, clip2.x, clip2.y);

	// find the longest diagonal (extend of disk around seed that must be drawn)
	Point diag1 = seed-clip1;
	Point diag2 = clip2-seed;
	Point ldiag(fmax(diag1.x,diag2.x),fmax(diag1.y,diag2.y));
	double r = ldiag.Norm();

	Cairo::RefPtr<Cairo::RadialGradient> gradient = Cairo::RadialGradient::create(seed.x, seed.y, 0, seed.x, seed.y, r);
	for (float d=0; d<=r; d+=delta)
		gradient->add_color_stop_rgba(d/r,0,0,0,grating->Render_(d,t));
	mmContext->set_source(gradient);
	mmContext->paint();

	return mmContext->pop_group();
}

Gtk::Widget *RadialGratingTexture::CreateWidget(TexturePtr ptr)
{
	RadialGratingTexturePtr gen = std::dynamic_pointer_cast<RadialGratingTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return Gtk::make_managed<RadialGratingTextureWidget>(gen);
}

Gtk::Widget *RadialGratingTexture::CreateAdvancedWidget(TexturePtr ptr)
{
	RadialGratingTexturePtr gen = std::dynamic_pointer_cast<RadialGratingTexture>(ptr);
	if (!gen) return Texture::CreateWidget(ptr); // default
	return Gtk::make_managed<RadialGratingTextureAdvancedWidget>(gen);
}

void RadialGratingTexture::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Texture");
	node.SetSubType("RadialGrating");
	GratingTexture::AppendXML(node);
}

void RadialGratingTexture::LoadXML(const XMLInputNode &node)
{
	GratingTexture::LoadXML(node);
}

std::string RadialGratingTexture::GetDefaultIconFile() const
{
	return tx_icon_radial_grating;
}

} // namespace
