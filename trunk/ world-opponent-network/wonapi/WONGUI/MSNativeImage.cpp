#include "MSNativeImage.h"
#include "MSRawImage.h"
#include "MSDisplayContext.h"
#include "MSGraphics.h"
#include "MSCursor.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSNativeMask::~MSNativeMask()
{
	DeleteObject(mMask); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSNativeIcon::~MSNativeIcon()
{
	DestroyIcon(mIcon);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr MSNativeIcon::GetImage()
{
	ICONINFO anInfo;
	if(!GetIconInfo(mIcon,&anInfo))
		return NULL;

	return new MSNativeImage(mDisplayContext, anInfo.hbmColor, new MSNativeMask(mDisplayContext, anInfo.hbmMask));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSNativeImage::SelectBitmap(bool select)
{
	if(mBitmapIsSelected==select)
		return;

	if(select)
		mOldBitmap = (HBITMAP)SelectObject(mDC, mBitmap);
	else
		SelectObject(mDC,mOldBitmap);

	mBitmapIsSelected = select;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSNativeImage::Init(DisplayContext *theContext, HBITMAP theBitmap, MSNativeMask *theTransparentMask)
{	
	mBitmap = theBitmap;
	mTransparentMask = theTransparentMask;
	mBitmapIsSelected = false;

	mDC = CreateCompatibleDC(((MSDisplayContext*)theContext)->mDC);
	mDisplayContext = ((MSDisplayContext*)theContext)->Duplicate();
	((MSDisplayContext*)mDisplayContext.get())->mDC = mDC;

	SelectBitmap(true);

	BITMAP     bm;
	POINT      size;
	GetObject(mBitmap, sizeof(BITMAP), (LPSTR)&bm);
	
	size.x = bm.bmWidth;            // Get width of bitmap
	size.y = bm.bmHeight;           // Get height of bitmap
	DPtoLP(mDC, &size, 1);			// Convert from 
	mWidth = size.x;
	mHeight = size.y;
	mImageData = bm.bmBits;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSNativeImage::MSNativeImage(DisplayContext *theContext, HBITMAP theBitmap, MSNativeMask *theTransparentMask) : NativeImage(theContext)
{
	Init(theContext, theBitmap, theTransparentMask);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSNativeImage::MSNativeImage(DisplayContext *theContext, int theWidth, int theHeight, bool useDibSection) : NativeImage(theContext)
{
	if(useDibSection)
	{
		BITMAPINFO anInfo;
		BITMAPINFOHEADER &aHeader = anInfo.bmiHeader;
		aHeader.biSize = sizeof(BITMAPINFOHEADER);
		aHeader.biWidth = theWidth;
		aHeader.biHeight = -theHeight;
		aHeader.biPlanes = 1;
		aHeader.biBitCount = 32;
		aHeader.biCompression = BI_RGB;
	
		void *anImageData = NULL;
		HBITMAP aBitmap = CreateDIBSection(((MSDisplayContext*)theContext)->mDC, &anInfo, DIB_RGB_COLORS, &anImageData, NULL, 0);
		Init(theContext,aBitmap,NULL);
	}
	else
		Init(theContext, CreateCompatibleBitmap(((MSDisplayContext*)theContext)->mDC,theWidth,theHeight), NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSNativeImage::~MSNativeImage()
{
	delete mGraphics;
	mGraphics = NULL;
	DeleteDC(mDC);
	DeleteObject(mBitmap);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HBITMAP MSNativeImage::GetHandle()
{
//	SelectBitmap(false);
	return mBitmap;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Graphics* MSNativeImage::PlatformGetGraphics()
{
	return new MSGraphics;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSNativeImage::Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height)
{
	if(!mBitmapIsSelected)
		SelectBitmap(true);

	HDC theDC = ((MSDisplayContext*)theContext)->mDC;
	if(mTransparentMask.get()!=NULL)
	{
		HDC aMaskDC = CreateCompatibleDC(theDC);
		SelectObject(aMaskDC,((MSNativeMask*)mTransparentMask.get())->GetMask());

		DWORD aTextColor = SetTextColor(theDC,0x000000); //off/non-transparent pixels -> black
		DWORD aBackColor = SetBkColor(theDC,0xFFFFFF);	// on/transparent pixels -> white
		
		BitBlt(theDC,x,y,width,height,aMaskDC,left,top,SRCAND);
		BitBlt(theDC,x,y,width,height,mDC,left,top,SRCPAINT);

		SetBkColor(theDC, aBackColor);
		SetTextColor(theDC, aTextColor);

		DeleteDC(aMaskDC);
	}
	else
		BitBlt(theDC, x, y, width, height, mDC, left, top, SRCCOPY);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImage32Ptr MSNativeImage::GetRaw()
{
	MSRawImage32Ptr anImage = new MSRawImage32(GetWidth(),GetHeight());
	
	SelectBitmap(false); // mBitmap can't be in mDC when I call GetDIBBits.

	char anInfoBuf[sizeof(BITMAPINFO) + 2*4];

	BITMAPINFO &anInfo = *(BITMAPINFO*)anInfoBuf;
	BITMAPINFOHEADER &aHeader = anInfo.bmiHeader;
	aHeader.biSize = sizeof(BITMAPINFOHEADER);
	aHeader.biWidth = GetWidth();
	aHeader.biHeight = -GetHeight();
	aHeader.biPlanes = 1;
	aHeader.biBitCount = 32;
	aHeader.biCompression = BI_RGB;

	GetDIBits(mDC, mBitmap, 0, GetHeight(), anImage->GetImageData(), &anInfo, DIB_RGB_COLORS); 

	if(mTransparentMask.get()!=NULL)
	{
		anImage->SetDoTransparency(true);
		MSRawImage1Ptr aMask = new MSRawImage1(GetWidth(),GetHeight());
		aHeader.biBitCount = 1;
		int aRet = GetDIBits(mDC, ((MSNativeMask*)mTransparentMask.get())->GetMask(), 0, GetHeight(), aMask->GetImageData(), &anInfo, DIB_RGB_COLORS); 
		aMask->StartRowTraversal(0);
		anImage->StartRowTraversal(0);
		for(int i=0; i<mHeight; i++)
		{
//			aMask->StartRowTraversal(i);
//			anImage->StartRowTraversal(i);
			while(anImage->HasMoreRowPixels())
			{
				bool val = aMask->GetRowPixel();
				if(!val)
					anImage->SetRowPixel(0xff000000 | anImage->GetRowPixel());

				anImage->NextRowPixel();
				aMask->NextRowPixel();
			}
			anImage->NextRow();
			aMask->NextRow();
		}
	}

	SelectBitmap(true);
	return anImage.get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr MSNativeImage::Duplicate()
{
	SelectBitmap(true);
	HDC aCopyDC = CreateCompatibleDC(mDC);
	HBITMAP aBitmap = CreateCompatibleBitmap(mDC,mWidth,mHeight);
	SelectObject(aCopyDC,aBitmap);
	BitBlt(aCopyDC,0,0,mWidth,mHeight,mDC,0,0,SRCCOPY);
	DeleteDC(aCopyDC);

	return new MSNativeImage(mDisplayContext, aBitmap, (MSNativeMask*)mTransparentMask.get());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CursorPtr MSNativeImage::GetCursor(int hotX, int hotY)
{
	ICONINFO anInfo;
	anInfo.fIcon = FALSE;
	anInfo.xHotspot = 0; 
	anInfo.yHotspot = 0; 
	anInfo.hbmMask = NULL;
	if(mTransparentMask.get()!=NULL)
		anInfo.hbmMask = ((MSNativeMask*)mTransparentMask.get())->GetMask();
   
	anInfo.hbmColor = mBitmap; 

	return new MSCursor((HCURSOR)CreateIconIndirect(&anInfo),true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeIconPtr MSNativeImage::GetIcon()
{
	ICONINFO anInfo;
	anInfo.fIcon = TRUE;
	anInfo.xHotspot = 0; 
	anInfo.yHotspot = 0; 
	anInfo.hbmMask = NULL;
	if(mTransparentMask.get()!=NULL)
		anInfo.hbmMask = ((MSNativeMask*)mTransparentMask.get())->GetMask();
   
	anInfo.hbmColor = mBitmap; 

	HICON anIcon = CreateIconIndirect(&anInfo);
	return new MSNativeIcon(mDisplayContext, anIcon);
}

