#include "XNativeImage.h"
#include "RawImage.h"
#include "XWindow.h"
#include "XGraphics.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XNativeMask::~XNativeMask()
{
	XFreePixmap(mDisplayContext->mDisplay, mMask);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XNativeImage::XNativeImage(DisplayContext *theContext, int theWidth, int theHeight, Pixmap thePixmap, NativeMask *theTransparentMask)
{		
	if(thePixmap==0)
		thePixmap = XCreatePixmap(theContext->mDisplay, theContext->mDrawable, theWidth, theHeight, theContext->mDepth);

	mWidth = theWidth;
	mHeight = theHeight;
	mPixmap = thePixmap;
	mTransparentMask = theTransparentMask;
	mDisplayContext = new DisplayContext(theContext);
	mDisplayContext->mDrawable = mPixmap;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XNativeImage::~XNativeImage()
{
	delete mGraphics;
	mGraphics = NULL;
	XFreePixmap(mDisplayContext->mDisplay, mPixmap);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Graphics* XNativeImage::PlatformGetGraphics()
{
	// Get independent GC for the DisplayContext (so SetColor doesn't affect other GCs/images
//	XGCValues aValues;
//	mDisplayContext->mGC = 	XCreateGC(mDisplayContext->mDisplay, mDisplayContext->mDrawable, 0, &aValues);
	mDisplayContext->CreateNewGC();
	return new XGraphics;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XNativeImage::Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height)
{
	if(mTransparentMask.get()!=NULL)
	{
		XGCValues oldValues;
		XGetGCValues(theContext->mDisplay, theContext->mGC, GCFunction | GCForeground | GCBackground, &oldValues);
		
		XGCValues xgcvalues;
		xgcvalues.function = GXand;
		xgcvalues.foreground = 0xffffff;//theContext->mWhitePixel;
		xgcvalues.background = 0;//theContext->mBlackPixel;

		XChangeGC(theContext->mDisplay, theContext->mGC, GCFunction | GCForeground | GCBackground, &xgcvalues);

		XCopyPlane(theContext->mDisplay, ((XNativeMask*)mTransparentMask.get())->GetMask(), theContext->mDrawable, theContext->mGC, left, top, width, height, x, y, 1);

		xgcvalues.function = GXor;
		XChangeGC(theContext->mDisplay, theContext->mGC, GCFunction, &xgcvalues);
		XCopyArea(theContext->mDisplay, mPixmap, theContext->mDrawable, theContext->mGC, left, top, width, height, x, y);

//		xgcvalues.function = GXcopy;
		XChangeGC(theContext->mDisplay, theContext->mGC, GCFunction | GCForeground | GCBackground, &oldValues);
	}
	else
	{
		XCopyArea(theContext->mDisplay, mPixmap, theContext->mDrawable, theContext->mGC, left, top, width, height, x, y);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImage32Ptr XNativeImage::GetRaw()
{
	return NULL;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr XNativeImage::Duplicate()
{
	XNativeImagePtr anImage = new XNativeImage(mDisplayContext, mWidth, mHeight);

	XGCValues aValues;
	GC aGC = XCreateGC(mDisplayContext->mDisplay, mDisplayContext->mDrawable, 0, &aValues);
	
	XCopyArea(mDisplayContext->mDisplay, mPixmap, anImage->mPixmap, aGC, 0, 0, mWidth, mHeight, 0, 0);
	XFreeGC(mDisplayContext->mDisplay, aGC);
	
	anImage->mTransparentMask = mTransparentMask;
	
	return anImage.get();
}
