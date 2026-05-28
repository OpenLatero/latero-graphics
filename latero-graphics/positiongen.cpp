#include "positiongen.h"
#include <boost/thread.hpp>

namespace latero::graphics {

PositionGen::PositionGen(const latero::Tactograph *dev) :
	Generator(dev)
{
};

PositionGen::~PositionGen()
{
};

void PositionGen::DoRender_(const State *state, latero::BiasedImg &frame)
{
	PreRender_(state);

	ActuatorState s(*state);
	for (s.i=0; s.i<frame.SizeX(); s.i++)
	{ 
		for (s.j=0; s.j<frame.SizeY(); s.j++)
		{ 
			s.pos = state->actuatorPositions.Get(s.i,s.j);
			s.relativePos = s.pos - state->pos;
			frame.Set(s.i,s.j,Render_(s));
		}
	}

	PostRender_(state);
}

Glib::RefPtr<Gdk::Pixbuf> PositionGen::GetDeflectionMap(uint w, boost::posix_time::time_duration timeElapsed, double velMag, double velDir, RenderMode mode)
{
	double ws_x = Dev()->GetSurfaceWidth();
	double ws_y = Dev()->GetSurfaceHeight();
	uint h = w * ws_y/ws_x;

	Glib::RefPtr<Gdk::Pixbuf> buf =
		Gdk::Pixbuf::create(Gdk::Colorspace::RGB, false, 8, w, h);

	ActuatorState state;
	state.mode = mode;
	state.i = state.j = 0; 
	state.SetTimeElapsed(timeElapsed);
	state.velMag = velMag;
	state.velDir = velDir;

	uint rowstride = buf->get_rowstride();
	uint n_channels = buf->get_n_channels();
	guint8* pixels = buf->get_pixels();
	for (int pix_y=0; pix_y<(int)h; ++pix_y)	
	{
		LATERO_GRAPHICS_LOCK; // todo: too expensive?
		for (int pix_x=0; pix_x<(int)w; ++pix_x)
		{
			float x = (float)pix_x * (float)ws_x / (float)w;
			float y = (float)pix_y * (float)ws_y / (float)h;
			state.pos = Point(x,y);

			float v = Render_(state,true);

			v = (-v+1)*255.0; // we want rest (0) to be white

			guint8* basePixel = pixels + pix_y*rowstride + pix_x*n_channels;
			for (uint n=0; n<3; ++n)
				basePixel[n] = (unsigned char)v;
		}
		LATERO_GRAPHICS_UNLOCK;
		boost::this_thread::yield();
	}
	
	return buf;
}

} // namespace

