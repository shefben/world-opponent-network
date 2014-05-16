#ifndef __SURFACE_H__
#define __SURFACE_H__
#include "WONCommon/SmartPtr.h"
#include <assert.h>
//#include "WONGUI/GUIString.h"

namespace WONAPI
{
	
//class SFont;
//class SNativeMask;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum SurfaceFormat
{
	SurfaceFormat_32,
	SurfaceFormat_555,
	SurfaceFormat_565
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WON_PTR_FORWARD(SurfaceBase);
class SurfaceBase : public RefCount
{
protected:
	SurfaceFormat mFormat;
	void *mImageBase;
	int mWidth, mHeight;
	int mBytesPerRow;
	int mPixelsPerRow;

#ifndef WIN32_NOT_XBOX
	struct BITMAPINFO
	{
		DWORD unused;
	};
#endif

	char mBitmapInfoBuffer[sizeof(BITMAPINFO) + sizeof(DWORD)*2];
	BITMAPINFO &mBitmapInfo;
	int mClipX1, mClipY1, mClipX2, mClipY2;

	inline void DoClipRect(int &x, int &y, int &width, int &height);
	inline void DoClipRect(int &x, int &y, int &left, int &top, int &width, int &height);

	inline int CalcClipCode(int x, int y);

	SurfaceBase& operator=(const SurfaceBase&) { assert(false); return *this; }
	virtual ~SurfaceBase();

public:
	virtual void CopySurface(SurfaceBase *theSurface, int x, int y, int left, int top, int width, int height)=0;
	virtual void CopySurface(SurfaceBase *theSurface, const char *theMaskData, int x, int y, int left, int top, int width, int height)=0;
//	virtual void DrawString(SFont *theFont, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)=0;

	virtual DWORD GetPixel(int x, int y) = 0;
	virtual void PutPixel(int x, int y) = 0;
	virtual void SetPixel(int x, int y, DWORD theColor) = 0;
	virtual void DrawCircle(int x, int y, int r) = 0;
	virtual void FillCircle(int x, int y, int r) = 0;
	virtual void DrawEllipse(int x, int y, int width, int height) = 0;
	virtual void FillEllipse(int x, int y, int width, int height) = 0;
	virtual void DrawArc(int x, int y, int width, int height, int startAngle, int arcAngle) = 0;

	virtual void DrawHorzLine(int x1, int x2, int y) = 0;
	virtual void DrawVertLine(int x, int y1, int y2) = 0;
	virtual void DrawLine(int x1, int y1, int x2, int y2) = 0;

	virtual void DrawRect(int x, int y, int width, int height) = 0;
	virtual void FillRect(int x, int y, int width, int height) = 0;
	virtual void SetColor(DWORD theColor) = 0;

public:
	SurfaceBase();

	virtual void Create(int theWidth, int theHeight);
	virtual void Free();
	virtual int GetPixelSize()=0;
	SurfaceFormat GetSurfaceFormat() { return mFormat; }
	int GetBytesPerRow() { return mBytesPerRow; }
	int GetPixelsPerRow() { return mPixelsPerRow; }

#ifdef WIN32_NOT_XBOX
	void BlitToDC(HDC theDC, int x, int y, int left, int top, int width, int height);
	void BlitToDC(HDC theDC);
#endif

	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }

	void SetClipRect(int x, int y, int width, int height);
	void ClearClipRect();

	void* GetPixelsBase() { return mImageBase; }

	virtual SurfaceBasePtr Duplicate()=0;

	static void SetDefaultSurfaceFormat(SurfaceFormat theFormat);
	static SurfaceBasePtr GetNewSurface(SurfaceFormat theFormat);
	static SurfaceBasePtr GetNewDefaultSurface();

	int GetClipX1() { return mClipX1; }
	int GetClipX2() { return mClipX2; }
	int GetClipY1() { return mClipY1; }
	int GetClipY2() { return mClipY2; }
	void GetClipRect(int &cx1, int &cy1, int &cx2, int &cy2) { cx1 = mClipX1; cx2 = mClipX2; cy1 = mClipY1; cy2 = mClipY2; }
	inline bool NotClipped(int x, int y) { 	return x>=mClipX1 && y>=mClipY1 && x<=mClipX2 && y<=mClipY2; }
};
typedef SmartPtr<SurfaceBase> SurfaceBasePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
class Surface : public SurfaceBase
{
protected:
//	typedef unsigned short PixelType;
//	typedef DWORD PixelType;

	PixelType *mImage;
	PixelType mColor;
	
	void DrawLineClip(int x1, int y1, int x2, int y2);
	virtual ~Surface();

public:
	Surface(SurfaceFormat theFormat);

	virtual void Create(int theWidth, int theHeight);
	virtual void Free();
	virtual int GetPixelSize() { return sizeof(PixelType); }
	
	virtual void CopySurface(SurfaceBase *theSurface, int x, int y, int left, int top, int width, int height);
	virtual void CopySurface(SurfaceBase *theSurface, const char *theMaskData, int x, int y, int left, int top, int width, int height);
//	virtual void DrawString(SFont *theFont, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y);

	DWORD GetPixel(int x, int y);
	void PutPixel(int x, int y);
	void SetPixel(int x, int y, DWORD theColor);
	void DrawCircle(int x, int y, int r);
	void FillCircle(int x, int y, int r);
	void DrawEllipse(int x, int y, int width, int height);
	void FillEllipse(int x, int y, int width, int height);
	void DrawArc(int x, int y, int width, int height, int startAngle, int arcAngle);

	void DrawHorzLine(int x1, int x2, int y);
	void DrawVertLine(int x, int y1, int y2);
	void DrawLine(int x1, int y1, int x2, int y2);

	PixelType* GetPixels() { return mImage; }
	PixelType GetColor() { return mColor; }

	void DrawRect(int x, int y, int width, int height);
	void FillRect(int x, int y, int width, int height);
	void SetColor(DWORD theColor);

	inline void PutPixel(int x, int y, PixelType *thePtr)
	{
		if(NotClipped(x,y))
			*thePtr = mColor;
	}

	SurfaceBasePtr Duplicate();
};
typedef Surface<unsigned short> Surface16;
typedef Surface<DWORD> Surface32;

typedef SmartPtr<Surface16> Surface16Ptr;
typedef SmartPtr<Surface32> Surface32Ptr;

#define SURFACE_32_TO_555(x) ((((x)>>3)&0x1f) | ((((x)>>11)&0x1f)<<5) | ((((x)>>19)&0x1f)<<10))
#define SURFACE_555_TO_32(x) ((((x)&0x1f)<<3) | ((((x)>>5)&0x1f)<<11) | ((((x)>>10)&0x1f)<<19))

#define SURFACE_32_TO_565(x) ((((x)>>3)&0x1f) | ((((x)>>10)&0x3f)<<5) | ((((x)>>19)&0x1f)<<11))
#define SURFACE_565_TO_32(x) ((((x)&0x1f)<<3) | ((((x)>>5)&0x3f)<<10) | ((((x)>>11)&0x1f)<<19))


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
class Surface32 : public Surface<DWORD>
{
public:
	Surface32();
};
typedef SmartPtr<Surface32> Surface32Ptr;
*/

} // namespace WONAPI;

#endif