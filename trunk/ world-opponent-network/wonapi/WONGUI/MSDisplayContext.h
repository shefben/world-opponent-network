#ifndef __WON_MSDISPLAYCONTEXT_H__
#define __WON_MSDISPLAYCONTEXT_H__

#include "DisplayContext.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSDisplayContext : public DisplayContext
{
public:
	HDC mDC;

protected:
	virtual ~MSDisplayContext() { }

public:
	MSDisplayContext() : mDC(NULL) { }
	MSDisplayContext(HDC theDC) : mDC(theDC) { }
	
	void Attach(HDC theDC) { mDC = theDC; }

	virtual NativeImagePtr CreateImage(int theWidth, int theHeight);
	virtual RawImage1Ptr CreateRawImage1(int theWidth, int theHeight);
	virtual RawImage8Ptr CreateRawImage8(int theWidth, int theHeight, Palette *thePalette = NULL);
	virtual RawImage32Ptr CreateRawImage32(int theWidth, int theHeight);

	virtual DisplayContextPtr Duplicate() { return new MSDisplayContext(mDC); }
};
typedef WONAPI::SmartPtr<MSDisplayContext> MSDisplayContextPtr;

}; // namespace WONAPI

#endif