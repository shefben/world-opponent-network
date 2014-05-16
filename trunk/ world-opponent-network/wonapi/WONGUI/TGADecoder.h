#ifndef __WON_TGADECODER_H__
#define __WON_TGADECODER_H__

#include "ImageDecoder.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TGADecoder : public ImageDecoder
{
protected:
	virtual RawImagePtr DoDecodeRaw();
	virtual bool DoGetAttrib(ImageAttributes &theAttrib);

public:
	virtual ImageDecoder* Duplicate() { return new TGADecoder; }
};

} // namespace WONAPI

#endif