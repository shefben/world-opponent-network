#include "RawImage.h"
#include "NativeImage.h"
#include "DisplayContext.h"
#include "NativeImage.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr RawImage32::PlatformGetNative(DisplayContext *theContext)
{
	XNativeImagePtr anImage = new XNativeImage(theContext,mWidth,mHeight);

	// Handle image
	XImage *anXImage = XCreateImage( theContext->mDisplay, theContext->mVisual, theContext->mDepth, ZPixmap, 0, 0, mWidth, mHeight, 32, 0 );
	anXImage->data = (char*)malloc(anXImage->bytes_per_line*anXImage->height);
	XInitImage(anXImage);
	
	
	DWORD *anImagePtr = mImageData;
	for(int y=0; y<mHeight; y++)
	{
		for(int x=0; x<mWidth; x++)
		{
			XPutPixel(anXImage,x,y,theContext->AllocColor(*anImagePtr));
			anImagePtr++;
		}
	}

	GC xgc = XCreateGC(theContext->mDisplay, anImage->GetHandle(), 0, NULL);
	XPutImage(theContext->mDisplay, anImage->GetHandle(), xgc, anXImage, 0,0,0,0,mWidth,mHeight);
	XFreeGC(theContext->mDisplay,xgc);
	XDestroyImage(anXImage);

	return anImage.get();
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RawImage8::CalcBytesPerRow()
{
	mBytesPerRow = mWidth;
//	if(mBytesPerRow%4!=0)
//		mBytesPerRow+=(4-mBytesPerRow%4);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr RawImage8::PlatformGetNative(DisplayContext *theContext)
{
	int aSize = mPalette->GetNumColors();
	DWORD aColorTable[256];	
	for(int i=0; i<aSize; i++)
		aColorTable[i] = theContext->AllocColor(mPalette->GetColor(i));
	
	if(mTransparentColor!=-1)
		aColorTable[mTransparentColor] = theContext->mBlackPixel;

	XNativeImagePtr anImage = new XNativeImage(theContext,mWidth,mHeight);

	XImage *anXImage = XCreateImage( theContext->mDisplay, theContext->mVisual, theContext->mDepth, ZPixmap, 0, 0, mWidth, mHeight, 32, 0 );
	anXImage->data = (char*)malloc(anXImage->bytes_per_line*anXImage->height);
	XInitImage(anXImage);
		

	unsigned char *anImagePtr = mImageData;
	for(int y=0; y<mHeight; y++)
	{
		for(int x=0; x<mWidth; x++)
		{
			XPutPixel(anXImage,x,y, aColorTable[*anImagePtr]);
			anImagePtr++;
		}
	}

	GC xgc = XCreateGC(theContext->mDisplay, anImage->GetHandle(), 0, NULL);
	XPutImage(theContext->mDisplay, anImage->GetHandle(), xgc, anXImage, 0,0,0,0,mWidth,mHeight);
	XFreeGC(theContext->mDisplay,xgc);
	XDestroyImage(anXImage);

	return anImage.get();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RawImage1::CalcBytesPerRow()
{
	mBytesPerRow = mBytesPerRowMinusPad;
//	if(mBytesPerRow%4!=0)
//		mBytesPerRow+=(4-mBytesPerRow%4);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeMaskPtr RawImage1::GetMask(DisplayContext *theContext)
{
//	return XCreateBitmapFromData(theContext->mDisplay, theContext->mDrawable, (char*)mImageData, mWidth, mHeight);
	Display *theDisplay = theContext->mDisplay;
	int theScreen = DefaultScreen(theDisplay);

	unsigned long white = theContext->mWhitePixel;
	unsigned long black = theContext->mBlackPixel;

	Pixmap aMask = XCreatePixmap(theDisplay, theContext->mDrawable, mWidth, mHeight, 1);

	GC xgc = XCreateGC(theDisplay, aMask, 0, NULL);

	StartRowTraversal(0);
	for(int y=0; y<mHeight; y++)
	{
		StartRowTraversal(y);
		for(int x=0; x<mWidth; x++)
		{
			if(GetRowPixel())
				XSetForeground(theDisplay, xgc, white);
			else
				XSetForeground(theDisplay, xgc, black);

			XDrawPoint(theDisplay, aMask, xgc, x, y);
			NextRowPixel();
		}
		NextRow();
	}

	XFreeGC(theDisplay, xgc);
	return new XNativeMask(aMask, theContext);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr RawImage1::GetNative(DisplayContext *theContext)
{
	return new XNativeImage(theContext,mWidth,mHeight,((XNativeMask*)GetMask(theContext).get())->GetMask());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
