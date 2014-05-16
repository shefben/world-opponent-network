#ifndef __WON_BMPDECODER_H__
#define __WON_BMPDECODER_H__

#include "NativeImage.h"
#include "ImageDecoder.h"
#include "WONCommon/FileReader.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BMPDecoder : public ImageDecoder
{
protected:
	virtual RawImagePtr DoDecodeRaw();
	virtual bool DoGetAttrib(ImageAttributes &theAttrib);

public:
	virtual ImageDecoder* Duplicate() { return new BMPDecoder; }


};

}; // namespace WONAPI

#endif