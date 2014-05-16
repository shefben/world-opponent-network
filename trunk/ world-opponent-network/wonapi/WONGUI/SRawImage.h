#ifndef __WON_SRAWIMAGE_H__
#define __WON_SRAWIMAGE_H__

#include "WONGUI/RawImage.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SRawImage32 : public RawImage32
{
protected:
	virtual NativeImagePtr PlatformGetNative(DisplayContext *theContext);
	virtual RawImage32* PlatformDuplicate();

public:
	SRawImage32(int theWidth, int theHeight);
};
typedef SmartPtr<SRawImage32> SRawImage32Ptr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SRawImage8 : public RawImage8
{
protected:
	virtual NativeImagePtr PlatformGetNative(DisplayContext *theContext);
	virtual RawImage8* PlatformDuplicate();

public:
	SRawImage8(int theWidth, int theHeight, Palette *thePalette = NULL);
};
typedef SmartPtr<SRawImage8> SRawImage8Ptr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SRawImage1 : public RawImage1
{
public:
	SRawImage1(int theWidth, int theHeight);

	virtual NativeMaskPtr GetMask(DisplayContext *theContext);
	virtual NativeImagePtr GetNative(DisplayContext *theContext);
};
typedef SmartPtr<SRawImage1> SRawImage1Ptr;


} // namespace WONAPI

#endif