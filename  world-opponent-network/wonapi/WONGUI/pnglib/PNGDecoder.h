#ifndef __WON_PNGDECODER_H__
#define __WON_PNGDECODER_H__

#include "../ImageDecoder.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PNGDecoder : public ImageDecoder
{
protected:
	virtual RawImagePtr DoDecodeRaw();
	virtual bool DoGetAttrib(ImageAttributes &theAttrib);

public:
	static void Add(MultiImageDecoder *theDecoder);

	virtual ImageDecoder* Duplicate() { return new PNGDecoder; }
};

};

#endif