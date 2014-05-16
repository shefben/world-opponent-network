#include "MSRawImage.h"
#include "MSNativeImage.h"
#include "MSDisplayContext.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSRawImage32::MSRawImage32(int theWidth, int theHeight) : RawImage32(theWidth, theHeight)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr MSRawImage32::PlatformGetNative(DisplayContext *theContext)
{
	BITMAPINFO anInfo;
	BITMAPINFOHEADER &aHeader = anInfo.bmiHeader;
	aHeader.biSize = sizeof(BITMAPINFOHEADER);
	aHeader.biWidth = GetWidth();
	aHeader.biHeight = -GetHeight();
	aHeader.biPlanes = 1;
	aHeader.biBitCount = 32;
	aHeader.biCompression = BI_RGB;

	HBITMAP aBitmap = CreateDIBitmap(((MSDisplayContext*)theContext)->mDC,&aHeader,CBM_INIT,mImageData,&anInfo,DIB_RGB_COLORS);
	return new MSNativeImage(theContext, aBitmap);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImage32* MSRawImage32::PlatformDuplicate()
{
	return new MSRawImage32(mWidth,mHeight);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSRawImage8::MSRawImage8(int theWidth, int theHeight, Palette *thePalette) : RawImage8(theWidth,theHeight,thePalette)
{
	mBytesPerRow = mWidth;
	if(mBytesPerRow%4!=0)
		mBytesPerRow+=(4-mBytesPerRow%4);

	FinishConstructor();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImage8* MSRawImage8::PlatformDuplicate()
{
	return new MSRawImage8(mWidth,mHeight,mPalette);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr MSRawImage8::PlatformGetNative(DisplayContext *theContext)
{
	char *anInfoBuf = new char[sizeof(BITMAPINFO) + mPalette->GetNumColors()*4];
	std::auto_ptr<char> aDelInfoBuf(anInfoBuf);

	BITMAPINFO *anInfo = (BITMAPINFO*)anInfoBuf;

	BITMAPINFOHEADER &aHeader = anInfo->bmiHeader;
	aHeader.biSize = sizeof(aHeader);
	aHeader.biWidth = mWidth;
	aHeader.biHeight = -mHeight;
	aHeader.biPlanes = 1;
	aHeader.biBitCount = 8;
	aHeader.biCompression = BI_RGB;
	aHeader.biSizeImage = 0;
	aHeader.biXPelsPerMeter = 0;
	aHeader.biYPelsPerMeter = 0;
	aHeader.biClrUsed = mPalette->GetNumColors();
	aHeader.biClrImportant = 0;

	int i;
	for(i=0; i<mPalette->GetNumColors(); i++)
	{
		unsigned long anRGB = mPalette->GetColor(i);
		RGBQUAD aQuad;
		aQuad.rgbRed = (anRGB>>16)&0xff;
		aQuad.rgbGreen = (anRGB>>8)&0xff;
		aQuad.rgbBlue = anRGB&0xff;
		aQuad.rgbReserved = 0;

		anInfo->bmiColors[i] = aQuad;
	}

	if(mTransparentColor!=-1 && mTransparentColor<mPalette->GetNumColors())
	{
		RGBQUAD aBlack = {0,0,0,0};
		anInfo->bmiColors[mTransparentColor] = aBlack;
	}

	HBITMAP aBitmap = CreateDIBitmap(((MSDisplayContext*)theContext)->mDC,&aHeader,CBM_INIT,mImageData,anInfo,DIB_RGB_COLORS);
	return new MSNativeImage(theContext, aBitmap);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSRawImage1::MSRawImage1(int theWidth, int theHeight) : RawImage1(theWidth, theHeight)
{
	mBytesPerRow = mBytesPerRowMinusPad;
	if(mBytesPerRow%4!=0)
		mBytesPerRow+=(4-mBytesPerRow%4);

	FinishConstructor();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeMaskPtr MSRawImage1::GetMask(DisplayContext *theContext)
{
	char *anInfoBuf = new char[sizeof(BITMAPINFO) + 2*4];
	std::auto_ptr<char> aDelInfoBuf(anInfoBuf);

	BITMAPINFO *anInfo = (BITMAPINFO*)anInfoBuf;

	BITMAPINFOHEADER &aHeader = anInfo->bmiHeader;
	aHeader.biSize = sizeof(aHeader);
	aHeader.biWidth = mWidth;
	aHeader.biHeight = -mHeight;
	aHeader.biPlanes = 1;
	aHeader.biBitCount = 1;
	aHeader.biCompression = BI_RGB;
	aHeader.biSizeImage = 0;
	aHeader.biXPelsPerMeter = 0;
	aHeader.biYPelsPerMeter = 0;
	aHeader.biClrUsed = 2;//mColorMap->GetSize();
	aHeader.biClrImportant = 0;

	RGBQUAD aBlack = {0,0,0,0};
	RGBQUAD aWhite = {0xff,0xff,0xff,0};
	anInfo->bmiColors[0] = aBlack;
	anInfo->bmiColors[1] = aWhite;

	HBITMAP aBitmap = CreateDIBitmap(((MSDisplayContext*)theContext)->mDC,&aHeader,CBM_INIT,mImageData,anInfo,DIB_RGB_COLORS);
	return new MSNativeMask(theContext, aBitmap);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr MSRawImage1::GetNative(DisplayContext *theContext)
{
	return new MSNativeImage(theContext,((MSNativeMask*)GetMask(theContext).get())->GetMask());
}

