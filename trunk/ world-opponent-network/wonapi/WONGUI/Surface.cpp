#include "Surface.h"

#include <math.h>

#include <algorithm>

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SurfaceBase::SurfaceBase() : mBitmapInfo(*(BITMAPINFO*)mBitmapInfoBuffer)
{
	mImageBase = NULL;
	mWidth = mHeight = 0;
	ClearClipRect();

	memset(&mBitmapInfo,0,sizeof(mBitmapInfo));

#ifdef WIN32_NOT_XBOX
	BITMAPINFOHEADER &aHeader = mBitmapInfo.bmiHeader;
	aHeader.biSize = sizeof(aHeader);
#endif
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SurfaceBase::~SurfaceBase()
{
	Free();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static SurfaceFormat gDefaultSurfaceFormat = SurfaceFormat_32;
void SurfaceBase::SetDefaultSurfaceFormat(SurfaceFormat theFormat)
{
	gDefaultSurfaceFormat = theFormat;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SurfaceBasePtr SurfaceBase::GetNewSurface(SurfaceFormat theFormat)
{
	switch(theFormat)
	{
		case SurfaceFormat_32: return new Surface32(theFormat); break;
		case SurfaceFormat_555: return new Surface16(theFormat); break;
		case SurfaceFormat_565: return new Surface16(theFormat); break;
		default: return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SurfaceBasePtr SurfaceBase::GetNewDefaultSurface()
{
	return GetNewSurface(gDefaultSurfaceFormat);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SurfaceBase::Create(int theWidth, int theHeight)
{
	Free();

#ifdef WIN32_NOT_XBOX
	mBitmapInfo.bmiHeader.biWidth = theWidth;
	mBitmapInfo.bmiHeader.biHeight = -theHeight;
#endif

	mBytesPerRow = theWidth*GetPixelSize();
	if(mBytesPerRow%4!=0)
		mBytesPerRow += 4-mBytesPerRow%4;

	mPixelsPerRow = mBytesPerRow/GetPixelSize();

	mImageBase = new char[mBytesPerRow*theHeight];

	mWidth = theWidth;
	mHeight = theHeight;

	ClearClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SurfaceBase::Free()
{
	delete [] (char*)mImageBase;
	mImageBase = NULL;
	mWidth = mHeight = 0;
	mBytesPerRow = 0;
	ClearClipRect();
}

#ifdef WIN32_NOT_XBOX
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SurfaceBase::BlitToDC(HDC theDC)
{
	SetDIBitsToDevice(theDC,0,0,mWidth,mHeight,0,0,0,mHeight,mImageBase,&mBitmapInfo,DIB_RGB_COLORS);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SurfaceBase::BlitToDC(HDC theDC, int x, int y, int left, int top, int width, int height)
{
	SetDIBitsToDevice(theDC,x,y,width,height,left,top,top,height,mImageBase,&mBitmapInfo,DIB_RGB_COLORS);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SurfaceBase::SetClipRect(int x, int y, int width, int height)
{
	mClipX1 = x;
	mClipY1 = y;
	mClipX2 = x+width-1;
	mClipY2 = y+height-1;

	if(mClipX1<0)
		mClipX1 = 0;
	if(mClipY1<0)
		mClipY1 = 0;
	if(mClipX2>=mWidth)
		mClipX2 = mWidth-1;
	if(mClipY2>=mHeight)
		mClipY2 = mHeight-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SurfaceBase::ClearClipRect()
{
	mClipX1 = mClipY1 = 0;
	mClipX2 = mWidth-1;
	mClipY2 = mHeight-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SurfaceBase::DoClipRect(int &x, int &y, int &width, int &height)
{
	int x2 = x+width-1, y2 = y+height-1;

	if(x<mClipX1) x = mClipX1; 
	if(x2>mClipX2) x2 = mClipX2;
	if(y<mClipY1) y = mClipY1; 
	if(y2>mClipY2) y2 = mClipY2;

	width = x2-x+1;
	height = y2-y+1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SurfaceBase::DoClipRect(int &x, int &y, int &left, int &top, int &width, int &height)
{
	int x2 = x+width-1, y2 = y+height-1;

	if(x<mClipX1) { left += mClipX1-x; x = mClipX1; }
	if(x2>mClipX2) x2 = mClipX2;
	if(y<mClipY1) { top += mClipY1-y; y = mClipY1; }
	if(y2>mClipY2) y2 = mClipY2;

	width = x2-x+1;
	height = y2-y+1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline int SurfaceBase::CalcClipCode(int x, int y)
{
	int aCode = 0;
	if(x<mClipX1)
		aCode |= 1;
	if(y<mClipY1)
		aCode |= 2;
	if(x>mClipX2)
		aCode |= 4;
	if(y>mClipY2)
		aCode |= 8;

	return aCode;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
Surface<PixelType>::Surface(SurfaceFormat theFormat)
{
	mImage = NULL;
	mColor = 0;
	mFormat = theFormat;

#ifdef WIN32_NOT_XBOX
	BITMAPINFOHEADER &aHeader = mBitmapInfo.bmiHeader;
	aHeader.biPlanes = 1;

	switch(mFormat)
	{
		case SurfaceFormat_555:
		case SurfaceFormat_565:
			aHeader.biBitCount = 16; 
			if(mFormat==SurfaceFormat_565)
				aHeader.biCompression = BI_BITFIELDS;
			else
				aHeader.biCompression = BI_RGB;

			*(DWORD*)&mBitmapInfo.bmiColors[0] = 0xf800;
			*(DWORD*)&mBitmapInfo.bmiColors[1] = 0x7e0;
			*(DWORD*)&mBitmapInfo.bmiColors[2] = 0x1f;
			break;
		
		case SurfaceFormat_32: 
			aHeader.biBitCount = 32; 
			aHeader.biCompression = BI_RGB;
			mFormat = SurfaceFormat_32; 
			break;
	}
#endif

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
Surface<PixelType>::~Surface()
{
	Free();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::Create(int theWidth, int theHeight)
{
	SurfaceBase::Create(theWidth, theHeight);
	mImage = (PixelType*)mImageBase;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::Free()
{
	SurfaceBase::Free();
	mImage = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
SurfaceBasePtr Surface<PixelType>::Duplicate()
{
	SmartPtr<Surface<PixelType> > aSurface = new Surface<PixelType>(mFormat);
	aSurface->Create(mWidth,mHeight);
	memcpy(aSurface->mImage,mImage,mBytesPerRow*mHeight);
	return aSurface.get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::CopySurface(SurfaceBase *theSurface, int x, int y, int left, int top, int width, int height)
{
	DoClipRect(x,y,left,top,width,height);
	if(width<=0 || height<=0)
		return;

	int srcPixelsPerRow = theSurface->GetPixelsPerRow();
	PixelType *srcRow = ((PixelType*)theSurface->GetPixelsBase()) + srcPixelsPerRow*top + left;
	PixelType *destRow = mImage + mPixelsPerRow*y + x;
	PixelType *src, *dest;

	for(int i=0; i<height; i++)
	{
		src = srcRow;
		dest = destRow;
		for(int j=0; j<width; j++)
			*dest++ = *src++;

		srcRow += srcPixelsPerRow;
		destRow += mPixelsPerRow;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::CopySurface(SurfaceBase *theSurface, const char *theMaskData, int x, int y, int left, int top, int width, int height)
{
	DoClipRect(x,y,left,top,width,height);
	if(width<=0 || height<=0)
		return;

	int srcWidth = theSurface->GetWidth();
	int srcPixlesPerRow = theSurface->GetPixelsPerRow();
	PixelType *srcRow = ((PixelType*)theSurface->GetPixelsBase()) + srcPixlesPerRow*top + left;
	PixelType *destRow = mImage + mPixelsPerRow*y + x;

	int aMaskBytesPerRow = srcWidth/8;
	if(srcWidth%8!=0)
		aMaskBytesPerRow++;

	int aMaskStartVal = 1<<(7-(left%8));
	unsigned char aMask;
	const unsigned char *aMaskRow = (const unsigned char*)theMaskData + aMaskBytesPerRow*top + left/8;

	PixelType *src, *dest;
	const unsigned char *aMaskPtr;

	for(int i=0; i<height; i++)
	{
		src = srcRow;
		dest = destRow;
		aMaskPtr = aMaskRow;
		aMask = aMaskStartVal;

		for(int j=0; j<width; j++)
		{
			if((*aMaskPtr) & aMask)
			{
				dest++;
				src++;
			}
			else
				*dest++ = *src++;

			aMask>>=1;
			if(aMask==0)
			{
				aMask = 128;
				aMaskPtr++;
			}
		}

		srcRow += srcPixlesPerRow;
		destRow += mPixelsPerRow;
		aMaskRow += aMaskBytesPerRow;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
template <class PixelType>
void Surface<PixelType>::DrawString(SFont *theFont, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	if(x>mClipX2 || y>mClipY2 || y+theFont->GetHeight()<=mClipY1)
		return;

	ReadBuffer aBuf;
	
	int aHeight = theFont->GetHeight();
	PixelType *anUpperLeftCorner = mImage + y*mPixelsPerRow + x;
	PixelType *aRow, *aPixel;
	for(int i=0; i<theLength; i++)
	{
		int aChar = theStr.at(theOffset+i);
		int aWidth;
		if(x>=mClipX1)
		{
			aBuf.SetData(theFont->GetCharData(aChar, &aWidth),1000000);

			aRow = anUpperLeftCorner; 
			for(int i=0; i<aHeight; i++)
			{
				aPixel = aRow;
				for(int j=0; j<aWidth; j++)
				{
					if(aBuf.ReadBit())
						PutPixel(x+j,y+i,aPixel);

					aPixel++;
				}
				aRow+=mPixelsPerRow;
			}
		}
		else
			aWidth=theFont->GetCharWidth(aChar);

		x+=aWidth;
		anUpperLeftCorner+=aWidth;

		if(x>mClipX2)
			return;
	}
}
*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline DWORD SurfaceColorFrom32(DWORD theColor, SurfaceFormat theFormat)
{
	switch(theFormat)
	{
		case SurfaceFormat_555: return SURFACE_32_TO_555(theColor); break;
		case SurfaceFormat_565: return SURFACE_32_TO_565(theColor); break;

		case SurfaceFormat_32:	
		default: 
			return theColor; 
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::SetColor(DWORD theColor)
{
	mColor = SurfaceColorFrom32(theColor,mFormat);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::FillRect(int x, int y, int width, int height)
{
	DoClipRect(x,y,width,height);
	if(width<=0 || height<=0)
		return;

	int i,j;
	PixelType *aRow = mImage + y*mPixelsPerRow + x;
	PixelType *aPtr;
	for(i=0; i<height; i++)
	{
		aPtr = aRow;   
		for(j=0; j<width; j++)
			*aPtr++ = mColor;

		aRow+=mPixelsPerRow;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::DrawRect(int x, int y, int width, int height)
{
	if(width==1)
	{
		// vert line special case
		DrawVertLine(x,y,y+height-1);
		return;
	}
	if(height==1)
	{
		// horz line special case
		DrawHorzLine(x,x+width-1,y);
		return;
	}

	DrawHorzLine(x,x+width-1,y);
	DrawHorzLine(x,x+width-1,y+height-1);
	DrawVertLine(x,y,y+height-1);
	DrawVertLine(x+width-1,y,y+height-1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::DrawHorzLine(int x1, int x2, int y)
{
	if(y<mClipY1 || y>mClipY2)
		return;

	if(x2<x1)
		std::swap(x2,x1);

	if(x1<mClipX1) x1 = mClipX1;
	if(x2>mClipX2) x2 = mClipX2;
	if(x2<x1)
		return;

	PixelType *aPtr = mImage + y*mPixelsPerRow + x1;   
	const int distance = x2-x1+1;
	for(int i=0; i<distance; i++)
		*aPtr++ = mColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::DrawVertLine(int x, int y1, int y2)
{
	if(x<mClipX1 || x>mClipX2)
		return;

	if(y2<y1)
		std::swap(y1,y2);

	if(y1<mClipY1) y1 = mClipY1;
	if(y2>mClipY2) y2 = mClipY2;
	if(y2<y1)
		return;

	PixelType *aPtr = mImage + y1*mPixelsPerRow + x;   
	const int distance = y2-y1+1;
	for(int i=0; i<distance; i++)
	{
		*aPtr = mColor;
		aPtr += mPixelsPerRow;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Midpoint line algorithm
//
// Line defined by y = (dy/dx)x + b, dx = (x1-x0), dy = (y1-y0)
// Let F(x,y) = dy*x - dx*y + c, c = dx*b
// Points on the line satisfy F(x,y) = 0.
//
// For each point x, y on the line we must determine whether the next point is (x+1, y) or (x+1,y+1)
// (for a line with a slope between 0 and 1.)  To decide, we evaluate
// F(x+1,y+1/2) and see whether it's above the line or below the line, i.e. positive or negative.
//
// Notice that 
// F(x+1,y) = F(x,y) + dy  and
// F(x+1,y+1) = F(x,y) + dy - dx.
// So the next decision variable can be computed in terms of the previous decision variable
//
// Note then that F(x0,y0) = 0 since (x0,y0) is a point on the line.
// So F(x0,y0+1/2) = F(x0,y0) - dx/2 = -dx/2.
//
// For all values to be integers make the decision variable 2*F(x+1,y+1/2).
// Note that I don't use the midpoint line algorithm
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::DrawLine(int x1, int y1, int x2, int y2)
{
	if(x1==x2) 
	{
		DrawVertLine(x1,y1,y2);
		return;
	}
	
	if(y1==y2)
	{
		DrawHorzLine(x1,x2,y1);
		return;
	}

	int clipCode1 = CalcClipCode(x1,y1);
	int clipCode2 = CalcClipCode(x2,y2);
	if(clipCode1!=0 || clipCode2!=0) // Need clipping
	{
		if((clipCode1&clipCode2)==0)
			DrawLineClip(x1,y1,x2,y2);
		// else line is completely outside clipping rectangle

		return;
	}


	int i;
	int accum = 0;

	int width = x2-x1;
	int height = y2-y1;
	const int xinc = width>0?1:-1;
	const int yinc = height>0?mPixelsPerRow:-mPixelsPerRow;
	if(width<0)
		width = -width;
	if(height<0)
		height = -height;

	PixelType *aPtr = mImage + y1*mPixelsPerRow + x1;
	if(width<height)
	{
		// up/down
		for(i=0; i<height; i++)
		{
			*aPtr = mColor;
			aPtr += yinc;
			accum += width;
			if(accum >= height)
			{
				aPtr += xinc;
				accum -= height;
			}
		}

	}
	else
	{
		// left/right
		for(i=0; i<width; i++)
		{
			*aPtr = mColor;
			aPtr += xinc;
			accum += height;
			if(accum >= width)
			{
				aPtr += yinc;
				accum -= width;
			}
		}
	}

	// Draw last point
	*aPtr = mColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline int my_round(float theNum) 
{
	return floorf(theNum + 0.5f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::DrawLineClip(int x1, int y1, int x2, int y2)
{
	int i;
	int accum = 0;

	int width = x2-x1;
	int height = y2-y1;
	const int xCoordInc = width>0?1:-1;
	const int yCoordInc = height>0?1:-1;
	const int xinc = xCoordInc;
	const int yinc = yCoordInc*mPixelsPerRow;

	if(width<0)
		width = -width;
	if(height<0)
		height = -height;

	PixelType *aPtr = mImage + y1*mPixelsPerRow + x1;
	int x=x1, y=y1;
	if(width<height)
	{
		// up/down
		for(i=0; i<height; i++)
		{
			if(NotClipped(x,y))
				*aPtr = mColor;

			aPtr += yinc;
			y += yCoordInc;
			accum += width;
			if(accum >= height)
			{
				aPtr += xinc;
				x += xCoordInc;
				accum -= height;
			}
		}

	}
	else
	{
		// left/right
		for(i=0; i<width; i++)
		{
			if(NotClipped(x,y))
				*aPtr = mColor;

			aPtr += xinc;
			x += xCoordInc;
			accum += height;
			if(accum >= width)
			{
				aPtr += yinc;
				y += yCoordInc;
				accum -= width;
			}
		}
	}

	// Draw last point
	if(NotClipped(x,y))
		*aPtr = mColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Midpoint circle algorithm
//
// Consider the circle of radius r at the origin defined by
// x^2 + y^2 = r^2
// Let F(x,y) = x^2 + y^2 - r^2.
// So F(x,y) = 0 if (x,y) is on the circle.
// F(x,y)>0 is outside the circle, and F(x,y)<0 is inside the circle.
//
// To draw the circle from 0 to 45 degrees, do the following.
// Given a point (x,y) on the circle the next point is either (x,y+1) or (x-1,y+1).
// To decide which consider F(x-1/2,y+1) to see which is closer.  If this is negative,
// then (x,y+1) is closer, if positive then (x-1,y+1) is closer.
//
// Notice that F(x,y+1) = x^2 + y^2 + 2y + 1 - r^2 = F(x,y) + 2y + 1.
// and F(x-1,y+1) = x^2 - 2x + 1 + y^2 + 2y + 1 - r^2 = F(x,y) + (2y - 2x) + 2.
//
// Start out F(x,y) as F(r,0) = 0.
// Then F(r-1/2, 1) = r^2 - r + 1/4 + 1 - r^2 
// 5/4 - r
// So use D(x,y) = 4*F(x,y) as the decision variable to avoid fractions.
// -> D(r-1/2,1) = 5 - 4r.
//
// Notice that  D(x,y+1) = 4*F(x,y+1) = 4*F(x,y) + 8y + 4 = D(x,y) + 8y + 4
// and D(x+1,y+1) = 4*F(x+1,y+1) = 4*F(x,y) + (8y - 8x) + 8

template <class PixelType>
void Surface<PixelType>::DrawCircle(int x, int y, int r)
{
	int x1 = r, y1 = 0;
	int d = 5 - 4*r;

	while(x1>=y1)
	{
		PutPixel(x + x1,y - y1); // (0-45)
		PutPixel(x + y1,y - x1); // (45-90)
		PutPixel(x - y1,y - x1); // (90-135)
		PutPixel(x - x1,y - y1); // (135,180)
		PutPixel(x - x1,y + y1); // (180,225)
		PutPixel(x - y1,y + x1); // (225,270)
		PutPixel(x + y1,y + x1); // (270,315)
		PutPixel(x + x1,y + y1); // (315,360);

		if(d>0) // Evalue D(x-1,y+1)
		{
			d = d + 8*(y1-x1) + 4;
			x1--;
			y1++;
		}
		else // Evalue D(x,y+1)
		{
			d = d + 8*y1 + 4;
			y1++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::FillCircle(int x, int y, int r)
{
	int x1 = r, y1 = 0;
	int d = 5 - 4*r;

	while(x1>y1)
	{
		DrawHorzLine(x-x1,x+x1,y-y1); // (0-45, 135-180)
		DrawHorzLine(x-x1,x+x1,y+y1); // (180-225, 315-360)
		DrawHorzLine(x-y1,x+y1,y-x1); // (45-90, 90-135)
		DrawHorzLine(x-y1,x+y1,y+x1); // (225-270,270-315)

		if(d>0) // Evalue D(x-1,y+1)
		{
			d = d + 8*(y1-x1) + 4;
			x1--;
			y1++;
		}
		else // Evalue D(x,y+1)
		{
			d = d + 8*y1 + 4;
			y1++;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Midpoint ellipse algorithm
//
// Consider the ellipse with horizontal radius, a, and vertical radius, b, at the origin defined by
// (x/a)^2 + (y/b)^2 = 1 --> (bx)^2 + (ay)^2 = (ab)^2
// Let F(x,y) = (bx)^2 + (ay)^2 - (ab)^2
// So F(x,y) = 0 if (x,y) is on the ellipse.
// F(x,y)>0 is outside the ellipse, and F(x,y)<0 is inside the ellipse.
//
// To draw the ellipse with slope<-1, do the following.
// Given a point (x,y) on the circle the next point is either (x,y+1) or (x-1,y+1).
// To decide which consider F(x-1/2,y+1) to see which is closer.  If this is negative,
// then (x,y+1) is closer, if positive then (x-1,y+1) is closer.
//
// Notice that F(x,y+1) = (bx)^2 + a^2(y^2 + 2y + 1) - (ab)^2 = F(x,y) + 2(a^2)y + a^2
// and F(x-1,y+1) = b^2(x^2 - 2x + 1) + a^2(y^2 + 2y + 1) - (ab)^2 = F(x,y) + 2((a^2)y - (b^2)x) + a^2 + b^2 
// and F(x-1,y) = b^2(x^2 - 2x + 1) + (ay)^2 - (ab^2) = F(x,y) - 2(b^2)x + b^2
//
// Start out F(x,y) as F(a,0) = 0.
// Then F(a-1/2, 1) = b^2(a^2 - a + 1/4) + a^2 - (ab)^2 = a^2 - (b^2)a + (b^2)*1/4
// 
// Again use D(x,y) = 4*F(x,y) as the decision variable.
// -> D(a-1/2,1) = 4a^2 - 4(b^2)a + b^2.
//
// Need to calculate point where slope is greater than -1. 
// -> gradiant of F(x,y) has i and j components equal.
// grad(F(x,y)) = df/dx i + df/dy j = 2(b^2)x i + 2(b^2)y j 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::DrawEllipse(int x, int y, int width, int height)
{
	typedef double IntType;
	IntType a = (IntType)(width-1)/2;
	IntType b = (IntType)(height-1)/2;
	IntType a2 = a*a;
	IntType b2 = b*b;
	IntType cx = x+a;
	IntType cy = y+b;

	IntType x1 = a, y1 = 0;
	IntType d = 4*a2 - 4*b2*a + b2;


	while(a2*y1 < b2*x1)
	{
//		double d2 = 4*(b2*(x1-0.5)*(x1-0.5) + a2*(y1+1)*(y1+1) - a2*b2);

		PutPixel(cx + x1,cy - y1); // (0-45)
		PutPixel(cx - x1,cy - y1); // (135,180)
		PutPixel(cx - x1,cy + y1); // (180,225)
		PutPixel(cx + x1,cy + y1); // (315,360);

		if(d>0) // Evalue D(x-1,y+1)
		{
			x1--;
			y1++;
			d = d + 8*(a2*y1 - b2*x1) + 4*a2;
		}
		else // Evalue D(x,y+1)
		{
			y1++;
			d = d + 8*a2*y1 + 4*a2;
		}
	}

	// Know D(x-1/2,y+1)  
	// Want D(x-1,y+1/2) 

	d = d + 4*(-a2*y1 - b2*x1 - a2 + b2) + a2 - b2;
	while(x1>=0)
	{
//		double d2 = 4*(b2*(x1-1)*(x1-1) + a2*(y1+0.5)*(y1+0.5) - a2*b2);
		
		PutPixel(cx + x1,cy - y1); // (45-90)
		PutPixel(cx - x1,cy - y1); // (90,135)
		PutPixel(cx - x1,cy + y1); // (225,270)
		PutPixel(cx + x1,cy + y1); // (270,315);

		if(d<0) // Evalue D(x-1,y+1)
		{
			x1--;
			y1++;
			d = d + 8*(a2*y1 - b2*x1) + 4*b2;
		}
		else // Evalue D(x-1,y)
		{
			x1--;
			d = d - 8*b2*x1 + 4*b2;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::FillEllipse(int x, int y, int width, int height)
{
	typedef double IntType;
	IntType a = (IntType)(width-1)/2;
	IntType b = (IntType)(height-1)/2;
	IntType a2 = a*a;
	IntType b2 = b*b;
	IntType cx = x+a;
	IntType cy = y+b;

	IntType x1 = a, y1 = 0;
	IntType d = 4*a2 - 4*b2*a + b2;


	while(a2*y1 < b2*x1)
	{
//		double d2 = 4*(b2*(x1-0.5)*(x1-0.5) + a2*(y1+1)*(y1+1) - a2*b2);

		DrawHorzLine(cx - x1, cx + x1, cy-y1);
		DrawHorzLine(cx - x1, cx + x1, cy+y1);

		if(d>0) // Evalue D(x-1,y+1)
		{
			x1--;
			y1++;
			d = d + 8*(a2*y1 - b2*x1) + 4*a2;
		}
		else // Evalue D(x,y+1)
		{
			y1++;
			d = d + 8*a2*y1 + 4*a2;
		}
	}

	d = d + 4*(-a2*y1 - b2*x1 - a2 + b2) + a2 - b2;
	while(x1>=0)
	{
//		double d2 = 4*(b2*(x1-1)*(x1-1) + a2*(y1+0.5)*(y1+0.5) - a2*b2);

		DrawHorzLine(cx - x1, cx + x1, cy-y1);
		DrawHorzLine(cx - x1, cx + x1, cy+y1);

		if(d<0) // Evalue D(x-1,y+1)
		{
			x1--;
			y1++;
			d = d + 8*(a2*y1 - b2*x1) + 4*b2;
		}
		else // Evalue D(x-1,y)
		{
			x1--;
			d = d - 8*b2*x1 + 4*b2;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct OctantEndPoint
{
	typedef double IntType;

	IntType x1, y1;
	IntType x2, y2;
	bool c1, c2;
	int mOctant;
	bool mInverseArc;

	enum { BIG_VAL = 100000 };

	void SetOctant(int theOctant, bool inverseArc) { mOctant = theOctant; mInverseArc = inverseArc; }
	void SetEnd1(IntType x, IntType y) { x1 = x; y1 = y; }
	void SetEnd2(IntType x, IntType y) { x2 = x; y2 = y; }

	void InvertLimits() { x1*=-1; y1*=-1; x2*=-1; y2*=-1; }
	void InitMax1() { x1 = y2 = BIG_VAL; x2 = y1 = -BIG_VAL; }
	void InitMax2() { x1 = y2 = -BIG_VAL; x2 = y1 = BIG_VAL; }

	void IncludeAll() 
	{ 
		switch(mOctant)
		{
			case 0: y1 = -BIG_VAL; y2 = BIG_VAL; break;
			case 1: x1 = BIG_VAL; x2 = -BIG_VAL; break;
			case 2: x1 = BIG_VAL; x2= -BIG_VAL; break;
			case 3: y1 = BIG_VAL; y2 = -BIG_VAL; break;
			case 4: y1 = BIG_VAL; y2 = -BIG_VAL; break;
			case 5: x1 = -BIG_VAL; x2 = BIG_VAL; break;
			case 6: x1 = -BIG_VAL; x2 = BIG_VAL; break;
			case 7: y1 = -BIG_VAL; y2 = BIG_VAL; break;
		}
	}

	void IncludeNone() 
	{
		IncludeAll();
		InvertLimits();
	}

	bool IncludePointPrv(IntType x, IntType y)
	{
		switch(mOctant)
		{
			case 0: return y>=y1 && y<=y2;
			case 1: return x<=x1 && x>=x2;
			case 2: return x<=x1 && x>=x2;
			case 3: return y<=y1 && y>=y2;
			case 4: return y<=y1 && y>=y2;
			case 5: return x>=x1 && x<=x2;
			case 6: return x>=x1 && x<=x2;
			case 7: return y>=y1 && y<=y2;
			default: return false;
		}
	}

	bool IncludePoint(IntType x, IntType y)
	{
		bool aVal = IncludePointPrv(x,y);
		if(mInverseArc)
			return !aVal;
		else
			return aVal;
	}
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::DrawArc(int x, int y, int width, int height, int startAngle, int arcAngle)
{
	if(arcAngle>=360)
	{
		DrawEllipse(x,y,width,height);
		return;
	}

	static const double PI = 3.14159;

	if(startAngle<0)
		startAngle = (startAngle+360)%360;
	else if(startAngle>360)
		startAngle%=360;

	bool inverseArc = false;
	int endAngle = startAngle + arcAngle;
	if(endAngle>360)
	{
		endAngle -= 360;
		std::swap(startAngle,endAngle);
		inverseArc = true;
	}
	
	typedef double IntType;
	IntType a = (IntType)(width-1)/2;
	IntType b = (IntType)(height-1)/2;
	IntType a2 = a*a;
	IntType b2 = b*b;
	IntType cx = x+a;
	IntType cy = y+b;

	IntType xEnd1 = a*cos(PI*startAngle/180);
	IntType yEnd1 = b*sin(PI*startAngle/180);
	IntType xEnd2 = a*cos(PI*endAngle/180);
	IntType yEnd2 = b*sin(PI*endAngle/180);

	OctantEndPoint oep[8];
	int curStartAngle = 0, curEndAngle = 45;
	for(int i=0; i<8; i++)
	{
		oep[i].SetOctant(i, inverseArc);
		if(curEndAngle<startAngle || curStartAngle>endAngle)
			oep[i].IncludeNone();
		else
		{
			oep[i].IncludeAll();
			if(curStartAngle<startAngle)
				oep[i].SetEnd1(xEnd1,yEnd1);
			if(curEndAngle>endAngle)
				oep[i].SetEnd2(xEnd2,yEnd2);
		}
		curStartAngle+=45;
		curEndAngle+=45;
	}

	IntType x1 = a, y1 = 0;
	IntType d = 4*a2 - 4*b2*a + b2;

	while(a2*y1 < b2*x1)
	{
		if(oep[0].IncludePoint(x1,y1)) PutPixel(cx + x1,cy - y1); // (0-45)
		if(oep[3].IncludePoint(-x1,y1)) PutPixel(cx - x1,cy - y1); // (135,180)
		if(oep[4].IncludePoint(-x1,-y1)) PutPixel(cx - x1,cy + y1); // (180,225)
		if(oep[7].IncludePoint(x1,-y1)) PutPixel(cx + x1,cy + y1); // (315,360);

		if(d>0) // Evalue D(x-1,y+1)
		{
			x1--;
			y1++;
			d = d + 8*(a2*y1 - b2*x1) + 4*a2;
		}
		else // Evalue D(x,y+1)
		{
			y1++;
			d = d + 8*a2*y1 + 4*a2;
		}
	}

	d = d + 4*(-a2*y1 - b2*x1 - a2 + b2) + a2 - b2;
	while(x1>=0)
	{
		if(oep[1].IncludePoint(x1,y1)) PutPixel(cx + x1,cy - y1); // (45-90)
		if(oep[2].IncludePoint(-x1,y1)) PutPixel(cx - x1,cy - y1); // (90,135)
		if(oep[5].IncludePoint(-x1,-y1)) PutPixel(cx - x1,cy + y1); // (225,270)
		if(oep[6].IncludePoint(x1,-y1)) PutPixel(cx + x1,cy + y1); // (270,315);

		if(d<0) // Evalue D(x-1,y+1)
		{
			x1--;
			y1++;
			d = d + 8*(a2*y1 - b2*x1) + 4*b2;
		}
		else // Evalue D(x-1,y)
		{
			x1--;
			d = d - 8*b2*x1 + 4*b2;
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::PutPixel(int x, int y)
{
	if(NotClipped(x,y))
		*(mImage + y*mPixelsPerRow + x) = mColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
template <class PixelType>
void Surface<PixelType>::PutPixel(int x, int y, PixelType *thePtr)
{
	if(NotClipped(x,y))
		*thePtr = mColor;
}
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
void Surface<PixelType>::SetPixel(int x, int y, DWORD theColor)
{
	if(NotClipped(x,y))
		*(mImage + y*mPixelsPerRow + x) = SurfaceColorFrom32(theColor,mFormat);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
DWORD Surface<PixelType>::GetPixel(int x, int y)
{
	if(x<0 || y<0 || x>=mWidth || y>=mHeight)
		return 0;

	return *(mImage + y*mPixelsPerRow + x);
}
