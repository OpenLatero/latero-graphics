// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#include "blobdetector.h"

namespace latero {
namespace graphics { 

namespace gtk {

BlobDetector::BlobDetector(Glib::RefPtr<Gdk::Pixbuf> buf) :
	img_(buf),
	h_(buf->get_height()),
	w_(buf->get_width()),
	buf_(buf->get_pixels()),
	rowstride_(buf->get_rowstride()),
	nchannels_(buf->get_n_channels())
{
	blobIds_ = new int*[w_];
	for(int i=0;i<w_;i++)
	{
		blobIds_[i] = new int[h_];
		for (int j=0; j<h_; ++j)
			blobIds_[i][j] = -1;
	}
}

BlobDetector::~BlobDetector()
{
	for(int x=0;x<w_;x++)
		delete[] blobIds_[x];
	delete[] blobIds_;
}

std::vector<BlobDetector::Blob> BlobDetector::GetBlobs()
{
	LabelPixels(); // pass 1
	RelabelPixels(); // pass2
	std::vector<BlobDetector::Blob> rv = GetBlobInfo();
	printf("found %d blobs\n", (int)rv.size());
	return rv;
}

std::vector<BlobDetector::Blob> BlobDetector::GetBlobInfo()
{
	std::vector<Point> com(equiv_.size(),Point(0,0));
	std::vector<float> value(equiv_.size(),0);
	std::vector<int> count(equiv_.size(),0);

	for (int x=0; x<w_; ++x)
	{
		for (int y=0; y<h_; ++y)
		{
			int id = blobIds_[x][y];
			if (id != -1)
			{
				com[id].x += x;
				com[id].y += y;
				value[id] += Pixel(x,y);
				count[id]++;
			}
		}
	}

	std::vector<Blob> rv;
	for (unsigned int id=0; id<equiv_.size(); ++id)
	{
		if (count[id])
		{
			com[id].x /= count[id];
			com[id].y /= count[id];
			value[id] /= count[id];

			Blob b;
			b.pos = com[id];
			b.value = value[id];
			rv.push_back(b);
		}
	}

	return rv;
}

void BlobDetector::LabelPixels()
{
	for (int y=0; y<h_; ++y)	
	{
		for (int x=0; x<w_; ++x)
		{
			if (Pixel(x,y))
			{
				// we only need to check the four neighbors that were already visited
				int neighbor[4];
				neighbor[0] = GetBlobId(x-1, y);
				neighbor[1] = GetBlobId(x-1, y-1);
				neighbor[2] = GetBlobId(x, y-1);
				neighbor[3] = GetBlobId(x+1, y-1);

				int my_id = -1;
				for (int i=0; i<4; ++i)
				{
					// for non-zero neighbors
					if (neighbor[i] != -1)
					{
						if (my_id == -1)
						{
							// this is the first neighbor with a blob id
							my_id = neighbor[i];
						}
						else
						{
							// we found another neighbor with a blob id
							SetEquivalence(neighbor[i], my_id);
						}
					}
				}

				// if none of my neighbors has a blob, create one
				if (my_id == -1)
					my_id = GetNewBlobID();

				// note this pixels's id
				blobIds_[x][y] = my_id;
			}
		}
	}
}

void BlobDetector::RelabelPixels()
{
	// insure that the mapping is always to the lowest equivalent
	for (unsigned int id=0; id<equiv_.size(); ++id)
	{
		unsigned int equiv = id;
		while (equiv_[equiv] != -1)
			equiv = equiv_[equiv];
		if (equiv != id)
			equiv_[id] = equiv;
	}

	// relabel blobs
	for (int y=0; y<h_; ++y)	
	{
		for (int x=0; x<w_; ++x)
		{
			int blob = blobIds_[x][y];
			if (blob != -1)
			{
				if (equiv_[blob] != -1)
					blobIds_[x][y] = equiv_[blob];
			}
		}
	}
}



} // namespace gtk

} // namespace graphics
} // namespace latero


