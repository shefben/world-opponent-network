#ifndef __WON_XNATIVEIMAGE_H__
#define __WON_XNATIVEIMAGE_H__

#include "NativeImage.h"
#include "Graphics.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XNativeMask : public NativeMask
{
protected:
	Pixmap mMask;

	~XNativeMask();

public:
	XNativeMask(Pixmap theMask, DisplayContext *theDisplayContext) : NativeMask(theDisplayContext), mMask(theMask) { }
	Pixmap GetMask() { return mMask; }
};
typedef SmartPtr<XNativeMask> XNativeMaskPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XNativeImage : public NativeImage
{
protected:
	Display *mDisplay;
	Pixmap mPixmap;	

	virtual ~XNativeImage();

	virtual Graphics* PlatformGetGraphics();

public:
	XNativeImage(DisplayContext *theContext, int theWidth, int theHeight, Pixmap thePixmap = 0, NativeMask* theTransparentMask = NULL);

	void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);
	
	RawImage32Ptr GetRaw();
	ImagePtr Duplicate();

	Pixmap GetHandle() { return mPixmap; }
};
typedef SmartPtr<XNativeImage> XNativeImagePtr;


}; // namespace WONAPI

#endif
