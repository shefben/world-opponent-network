#ifndef __WON_MSNATIVEIMAGE_H__
#define __WON_MSNATIVEIMAGE_H__
#include "NativeImage.h"

namespace WONAPI
{

WON_PTR_FORWARD(Cursor);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSNativeMask : public NativeMask
{
protected:
	HBITMAP mMask;

	virtual ~MSNativeMask();

public:
	MSNativeMask(DisplayContext *theContext, HBITMAP theMask) : NativeMask(theContext), mMask(theMask) { }
	HBITMAP GetMask() { return mMask; }
};
typedef SmartPtr<MSNativeMask> MSNativeMaskPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSNativeIcon : public NativeIcon
{
protected:
	HICON mIcon;

	virtual ~MSNativeIcon();

public:
	MSNativeIcon(DisplayContext *theContext, HICON theIcon) : NativeIcon(theContext), mIcon(theIcon) { }
	HICON GetIcon() { return mIcon; }
	NativeImagePtr GetImage();
};
typedef SmartPtr<MSNativeIcon> MSNativeIconPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSNativeImage : public NativeImage
{
protected:
	HDC mDC;
	HBITMAP mBitmap;
	HBITMAP mOldBitmap;
	bool mBitmapIsSelected;
	void *mImageData;

	virtual ~MSNativeImage();

	void Init(DisplayContext *theContext, HBITMAP theBitmap, MSNativeMask *theTransparentMask);
	virtual Graphics* PlatformGetGraphics();

public:
	MSNativeImage(DisplayContext *theContext, int theWidth, int theHeight, bool useDibSection = false);
	MSNativeImage(DisplayContext *theContext, HBITMAP theBitmap, MSNativeMask *theTransparentMask = NULL);

	void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);

	virtual CursorPtr GetCursor(int hotX = 0, int hotY = 0);
	virtual NativeIconPtr GetIcon();
	virtual RawImage32Ptr GetRaw();
	virtual ImagePtr Duplicate();

	HBITMAP GetHandle();
	void* GetImageData() { return mImageData; }
	void SelectBitmap(bool select);
};
typedef SmartPtr<MSNativeImage> MSNativeImagePtr;

}; // namespace WONAPI


#endif