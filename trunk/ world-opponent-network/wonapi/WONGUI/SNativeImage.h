#ifndef __WON_SNATIVEIMAGE_H__
#define __WON_SNATIVEIMAGE_H__

#include "WONCommon/ByteBuffer.h"
#include "WONGUI/NativeImage.h"
#include "Surface.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SNativeMask : public NativeMask
{
protected:
	virtual ~SNativeMask();
	ByteBufferPtr mData;

public:
	SNativeMask(DisplayContext *theContext, const ByteBuffer *theData) : NativeMask(theContext), mData(theData) { }

	const char* GetData() { return mData->data(); }
};
typedef SmartPtr<SNativeMask> SNativeMaskPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SNativeImage : public NativeImage
{
protected:
	SurfaceBasePtr mSurface;

	virtual ~SNativeImage();

	virtual Graphics* PlatformGetGraphics();

public:
	SNativeImage(DisplayContext *theContext, int theWidth, int theHeight);
	SNativeImage(SurfaceBase *theSurface);

	void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);

	virtual RawImage32Ptr GetRaw();
	virtual ImagePtr Duplicate();
};
typedef SmartPtr<SNativeImage> SNativeImagePtr;

}; // namespace WONAPI


#endif