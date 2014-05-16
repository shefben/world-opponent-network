#ifndef __WON_MSRAWIMAGE_H__
#define __WON_MSRAWIMAGE_H__

#include "RawImage.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSRawImage32 : public RawImage32
{
protected:
	virtual NativeImagePtr PlatformGetNative(DisplayContext *theContext);
	virtual RawImage32* PlatformDuplicate();

public:
	MSRawImage32(int theWidth, int theHeight);
};
typedef SmartPtr<MSRawImage32> MSRawImage32Ptr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSRawImage8 : public RawImage8
{
protected:
	virtual NativeImagePtr PlatformGetNative(DisplayContext *theContext);
	virtual RawImage8* PlatformDuplicate();

public:
	MSRawImage8(int theWidth, int theHeight, Palette *thePalette = NULL);
};
typedef SmartPtr<MSRawImage8> MSRawImage8Ptr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSRawImage1 : public RawImage1
{
public:
	MSRawImage1(int theWidth, int theHeight);

	virtual NativeMaskPtr GetMask(DisplayContext *theContext);
	virtual NativeImagePtr GetNative(DisplayContext *theContext);
};
typedef SmartPtr<MSRawImage1> MSRawImage1Ptr;


} // namespace WONAPI

#endif