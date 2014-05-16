#ifndef __WON_MSBMPDECODER_H__
#define __WON_MSBMPDECODER_H__

#include "BMPDecoder.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSBMPDecoder : public BMPDecoder
{
protected:
	virtual NativeImagePtr DoDecode(DisplayContext *theContext);

public:
	virtual ImageDecoder* Duplicate() { return new MSBMPDecoder; }

	static void SetUseMSDecoder(bool useMS);
};

}; // namespace WONAPI

#endif