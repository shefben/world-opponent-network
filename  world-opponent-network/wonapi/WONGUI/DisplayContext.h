#ifndef __WON_DISPLAYCONTEXT_H__
#define __WON_DISPLAYCONTEXT_H__
#include "WONCommon/SmartPtr.h"

namespace WONAPI
{

class Graphics;
class Palette;
WON_PTR_FORWARD(NativeImage);
WON_PTR_FORWARD(RawImage1);
WON_PTR_FORWARD(RawImage8);
WON_PTR_FORWARD(RawImage32);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DisplayContext : public WONAPI::RefCount
{
public:
	Graphics *mGraphics;

protected:
	virtual ~DisplayContext() { }

public:
	DisplayContext() : mGraphics(NULL) { }
	
	virtual NativeImagePtr CreateImage(int theWidth, int theHeight) = 0;
	virtual RawImage1Ptr CreateRawImage1(int theWidth, int theHeight) = 0;
	virtual RawImage8Ptr CreateRawImage8(int theWidth, int theHeight, Palette *thePalette = NULL) = 0;
	virtual RawImage32Ptr CreateRawImage32(int theWidth, int theHeight) = 0;
};
typedef WONAPI::SmartPtr<DisplayContext> DisplayContextPtr;

} // namespace WONAPI

#endif
