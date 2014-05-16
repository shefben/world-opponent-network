#ifndef __WON_JPGDECODER_H__
#define __WON_JPGDECODER_H__

#include "../ImageDecoder.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JPGDecoder : public ImageDecoder
{
protected:
	virtual RawImagePtr DoDecodeRaw();
	virtual bool DoGetAttrib(ImageAttributes &theAttrib);

public:
	static void Add(MultiImageDecoder *theDecoder);

	virtual ImageDecoder* Duplicate() { return new JPGDecoder; }
};

};

#endif