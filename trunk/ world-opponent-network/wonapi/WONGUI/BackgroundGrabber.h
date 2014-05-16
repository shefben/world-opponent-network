#ifndef __WON_BACKGROUNDGRABBER_H__
#define __WON_BACKGROUNDGRABBER_H__

#include "Component.h"
#include "NativeImage.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BackgroundGrabber
{
public:
	NativeImagePtr mBGImage;
	int mImageX, mImageY;
	WONRectangle mClipRect;
	bool mNeedPutBG, mHaveBG;
	int mTranslucentColor, mTranslucentLevel;
	WindowPtr mDebugWindow;
	unsigned __int64 mImageOrder;
	static unsigned __int64 mImageOrderCounter;

public:
	BackgroundGrabber();
	void SizeChanged(Component *theComponent);
	void PrePaint(Component *theComponent, Graphics &g);
	void Paint(Component *theComponent, Graphics &g);
	void Invalidate(Component *theComponent);
	void Clear();
	void SetTranslucentColor(int theColor, int theLevel);

	bool NeedDrawBG() { return mHaveBG && mNeedPutBG; }
};

}; // namespace WONAPI

#endif