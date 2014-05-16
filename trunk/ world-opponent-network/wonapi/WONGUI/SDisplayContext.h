#ifndef __WON_SDISPLAYCONTEXT_H__
#define __WON_SDISPLAYCONTEXT_H__

#include "WONGUI/DisplayContext.h"
#include "Surface.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SDisplayContext : public DisplayContext
{
protected:
	virtual ~SDisplayContext() { }

public:
	SurfaceBasePtr mSurface;
	HDC mDC;

	SDisplayContext();
	SDisplayContext(SurfaceBase *theSurface);
	SDisplayContext(HDC theDC);

	virtual NativeImagePtr CreateImage(int theWidth, int theHeight);
	virtual RawImage1Ptr CreateRawImage1(int theWidth, int theHeight);
	virtual RawImage8Ptr CreateRawImage8(int theWidth, int theHeight, Palette *thePalette = NULL);
	virtual RawImage32Ptr CreateRawImage32(int theWidth, int theHeight);
};
typedef WONAPI::SmartPtr<SDisplayContext> SDisplayContextPtr;

}; // namespace WONAPI

#endif