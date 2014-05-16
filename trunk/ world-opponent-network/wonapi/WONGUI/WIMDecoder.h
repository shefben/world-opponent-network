#ifndef __WON_WIMDECODER_H__
#define __WON_WIMDECODER_H__

#include "WONCommon/ByteBuffer.h"
#include "ImageDecoder.h"

namespace WONAPI
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WIMDecoder : public ImageDecoder
{
protected:
	virtual RawImagePtr DoDecodeRaw();
	virtual bool DoGetAttrib(ImageAttributes &theAttrib);

public:
	static bool EncodeToFile(RawImagePtr theImage, const char *theFilePath);
	static ByteBufferPtr EncodeToBuffer(RawImagePtr theImage);

	virtual ImageDecoder* Duplicate() { return new WIMDecoder; }
};
typedef SmartPtr<WIMDecoder> WIMDecoderPtr;


}; // namespace WONAPI



#endif