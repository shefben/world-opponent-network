#ifndef __WON_NATIVEIMAGE_H__
#define __WON_NATIVEIMAGE_H__

#include "Image.h"
#include "Graphics.h"

namespace WONAPI
{
class DisplayContext;
typedef SmartPtr<DisplayContext> DisplayContextPtr;

class Graphics;

class RawImage32;
typedef WONAPI::SmartPtr<RawImage32> RawImage32Ptr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class NativeMask : public WONAPI::RefCount
{
protected:
	DisplayContextPtr mDisplayContext;
	virtual ~NativeMask();

public:
	NativeMask(DisplayContext *theDisplayContext = NULL);
};
typedef WONAPI::SmartPtr<NativeMask> NativeMaskPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class NativeIcon : public WONAPI::RefCount
{
protected:
	DisplayContextPtr mDisplayContext;
	virtual ~NativeIcon();

public:
	NativeIcon(DisplayContext *theContext = NULL);
};
typedef WONAPI::SmartPtr<NativeIcon> NativeIconPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class NativeImage : public Image
{
protected:
	Graphics *mGraphics;
	DisplayContextPtr mDisplayContext;
	NativeMaskPtr mTransparentMask;

	virtual ~NativeImage();
	virtual Graphics* PlatformGetGraphics() = 0;

public:
	NativeImage(DisplayContext *theContext = NULL);

	void SetTransparentMask(NativeMask *theMask) { mTransparentMask = theMask; }
	bool IsTransparent() { return mTransparentMask.get()!=NULL; }
	DisplayContext* GetDisplayContext() { return mDisplayContext; }
	
	virtual NativeIconPtr GetIcon() { return NULL; }
	virtual RawImage32Ptr NativeImage::GetRaw();
	virtual NativeImagePtr GetNative(DisplayContext *theContext);

	virtual void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height) = 0;
	Graphics& GetGraphics();
};
typedef WONAPI::SmartPtr<NativeImage> NativeImagePtr;

};


#endif