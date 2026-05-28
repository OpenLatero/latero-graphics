#include "modulator.h"
#include "dirmodulator.h"
#include "velmodulator.h"

namespace latero::graphics {

ModulatorPtr Modulator::Create(const latero::Tactograph *dev, const XMLInputNode &node)
{
	assert(node.GetName() == "Modulator");
	std::string type = node.GetType();
	if (type == DirModulator::GetXMLType())		return DirModulator::Create(dev,node);
	else if (type == VelModulator::GetXMLType())	return VelModulator::Create(dev,node);
	else
	{
		printf("Error: Attempted to create a modulator of unknown type %s\n", type.c_str());
		return ModulatorPtr();
	}
}


Gtk::Widget *Modulator::CreateWidget(ModulatorPtr ptr)
{
	return Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
}

Glib::RefPtr<Gdk::Pixbuf> Modulator::GetIllustration(uint size, double maxVel)
{
	Glib::RefPtr<Gdk::Pixbuf> rv = Gdk::Pixbuf::create(Gdk::Colorspace::RGB, true, 8, size, size);

	uint rowstride = rv->get_rowstride();
	uint n_channels = rv->get_n_channels();
	guint8* pixels = rv->get_pixels();
	int radius = size/2;

	ActuatorState simState;
	simState.mode = RenderMode(true,true,false);

	for (unsigned int y=0; y<size; ++y)
	{
		for (unsigned int x=0; x<size; ++x)
		{
			guint8* pix = pixels + y*rowstride + x*n_channels;
			int dx = x-radius;
			int dy = y-radius;
			double dr = Point(dx,dy).Norm();
			simState.velDir = atan2(dy,dx);
			simState.velMag = dr * maxVel / radius;
			unsigned char v = (unsigned char)(255*GetValue_(simState));
			pix[0] = pix[1] = pix[2] = v;
			pix[3] = (dr <= radius)? 255 : 0;
		}
	}
	return rv;
}

} // namespace



