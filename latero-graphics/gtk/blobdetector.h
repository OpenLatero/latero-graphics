#pragma once

#include <gdkmm/pixbuf.h>
#include <latero/point.h>

namespace latero::graphics::gtk {

/**
 * This class computes the center of mass and mean pixel value of blobs in a grayscale image.
 * A blob is defined as a group of 8-connected non-zero pixels.
 */
class BlobDetector
{
public:
	typedef struct
	{
		Point pos;
		float value;
	} Blob;

public:
	BlobDetector(Glib::RefPtr<Gdk::Pixbuf> buf);
	virtual ~BlobDetector();

	/**
	 * @returns A list of blobs with their center of mass and mean pixel value. A blob is defined as a group of 
	 * non-zero, 8-connected pixels.
	 */
	std::vector<Blob> GetBlobs();
	
protected:

	/** Compute the center of mass and mean pixel value of the blobs. */
	std::vector<Blob> GetBlobInfo();


	/**
	 * Pass 1: Go through the image and assign blob IDs to non-zero pixels. If blobs are found to be
	 * connected along the way, make a note of the equivalence for later correction.
	 */
	void LabelPixels();

	/** Pass 2: Go through the image again and replace all blob IDs by the lowest equivalent if applicable. */
	void RelabelPixels();

	/** Make a note that max and min blob IDs are equivalent. */
	inline void SetEquivalence(int min, int max)
	{
		if (min == max)
		{
			// if identical, do nothing
			return;
		}
		else if (min > max)
		{
			// if inverted, correct
			int tmp = max;
			max = min;
			min = tmp;
		}

		if (equiv_[max] != -1)
			SetEquivalence(equiv_[max], min);
		equiv_[max] = min;
	}

	/** Obtain a new blob id. */
	inline int GetNewBlobID()
	{
		equiv_.push_back(-1);
		return static_cast<int>(equiv_.size())-1;
	}

	/** @returns blob id at a given position (or -1 if unset or out of bounds) */
	inline int GetBlobId(int x, int y)
	{
		return Valid(x,y)?blobIds_[x][y]:-1;
	}

	/** @return if the coordinates are whithin the bounds of the image */
	inline bool Valid(int x, int y)
	{
		return ((x>=0)&&(x<w_)&&(y>=0)&&(y<h_));
	}

	/** @returns value of image pixel at position x,y */
	inline int Pixel(int x, int y)
	{
		guint8* p = buf_ + y*rowstride_ + x*nchannels_;
		return p[0];
	}


	Glib::RefPtr<Gdk::Pixbuf> img_;	// image
	int h_, w_; 			//
	guint8* buf_; 			// image information 
	int rowstride_;			// (to speed things up)	
	int nchannels_;			//

	int **blobIds_;			// blob ID for each pixel
	std::vector<int> equiv_; 	// equivalence map (blob id -> smaller equivalent blob id)
};

} // namespace

