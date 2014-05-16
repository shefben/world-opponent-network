#ifndef __WON_XDISPLAYCONTEXT_H__
#define __WON_XDISPLAYCONTEXT_H__

#include "WONCommon/SmartPtr.h"

namespace WONAPI
{
class Graphics;
class NativeImage;
typedef SmartPtr<NativeImage> NativeImagePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GCHolder : public RefCount
{
public:
	GC mGC;
	Display *mDisplay;
	
	GCHolder(Display *theDisplay, GC theGC) : mGC(theGC), mDisplay(theDisplay) { }
	virtual ~GCHolder();
};
typedef SmartPtr<GCHolder> GCHolderPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DisplayContext : public WONAPI::RefCount
{
public:
	Display *mDisplay;
	Visual *mVisual;
	int mScreen;
	int mDepth;
	Drawable mDrawable;
	GCHolderPtr mGCHolder;
	GC mGC;
	unsigned long mWhitePixel;
	unsigned long mBlackPixel;
	Colormap mColormap;

	Graphics *mGraphics;

protected:
	virtual ~DisplayContext();

public:
	DisplayContext() : mDisplay(NULL), mVisual(NULL), mScreen(0), mDepth(0), mDrawable(0), mGC(0), mWhitePixel(0), mBlackPixel(0), mColormap(0), mGraphics(NULL) { }
	DisplayContext(DisplayContext *c) : mDisplay(c->mDisplay), mVisual(c->mVisual), mScreen(c->mScreen), mDepth(c->mDepth), mDrawable(c->mDrawable), mGC(c->mGC), mGCHolder(c->mGCHolder), mWhitePixel(c->mWhitePixel), mBlackPixel(c->mBlackPixel), mColormap(c->mColormap), mGraphics(c->mGraphics) { }

	DWORD AllocColor(DWORD theColor);
	static void StaticInit(DisplayContext *theContext);

	NativeImagePtr CreateImage(int theWidth, int theHeight);
	void CreateNewGC();
};
typedef WONAPI::SmartPtr<DisplayContext> DisplayContextPtr;

}; // namespace WONAPI

#endif
