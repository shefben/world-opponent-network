#ifndef __WON_SURFACEIMAGE_H__
#define __WON_SURFACEIMAGE_H__
#include "WONGUI/Image.h"
#include "WONGUI/Surface.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SurfaceImage : public Image
{
protected:
	SurfaceBasePtr mSurface;
	Graphics *mGraphics;

	virtual ~SurfaceImage(); 

public:

	SurfaceImage(SurfaceBase *theSurface);
	virtual void Draw(DisplayContext *theContext,int x, int y, int left, int top, int width, int height);

	Graphics& GetGraphics();
	SurfaceBase* GetSurface() { return mSurface; }
};
typedef SmartPtr<SurfaceImage> SurfaceImagePtr;

} // namespace WONAPI

#endif