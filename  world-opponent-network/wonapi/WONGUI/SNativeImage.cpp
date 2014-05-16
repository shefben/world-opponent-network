#include "SNativeImage.h"
#include "SRawImage.h"
#include "SDisplayContext.h"
#include "SGraphics.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SNativeMask::~SNativeMask()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SNativeImage::SNativeImage(DisplayContext *theContext, int theWidth, int theHeight) : NativeImage(theContext)
{
	mSurface = SurfaceBase::GetNewDefaultSurface();
	mSurface->Create(theWidth,theHeight);
	mDisplayContext = new SDisplayContext(mSurface);

	mWidth = theWidth;
	mHeight = theHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SNativeImage::SNativeImage(SurfaceBase *theSurface) : mSurface(theSurface)
{
	mDisplayContext = new SDisplayContext(mSurface);
	mWidth = theSurface->GetWidth();
	mHeight = theSurface->GetHeight();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SNativeImage::~SNativeImage()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Graphics* SNativeImage::PlatformGetGraphics()
{
	return new SGraphics;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SNativeImage::Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height)
{
	if(((SDisplayContext*)theContext)->mSurface.get()!=NULL)
	{
		if(mTransparentMask.get()!=NULL)
			((SDisplayContext*)theContext)->mSurface->CopySurface(mSurface,((SNativeMask*)mTransparentMask.get())->GetData(),x,y,left,top,width,height);
		else
			((SDisplayContext*)theContext)->mSurface->CopySurface(mSurface,x,y,left,top,width,height);
	}
#ifdef WIN32_NOT_XBOX
	else if(((SDisplayContext*)theContext)->mDC!=NULL)
		mSurface->BlitToDC(((SDisplayContext*)theContext)->mDC,x,y,left,top,width,height);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImage32Ptr SNativeImage::GetRaw()
{
	SRawImage32Ptr anImage = new SRawImage32(mWidth,mHeight);
	switch(mSurface->GetSurfaceFormat())
	{
		case SurfaceFormat_555:
		{
			unsigned char *row = (unsigned char*)mSurface->GetPixelsBase();
			DWORD *dest = anImage->GetImageData();
			int aNumPixels = mSurface->GetWidth()*mSurface->GetHeight();
			for(int i=0; i<mHeight; i++)
			{
				unsigned short *src = (unsigned short*)row;
				for(int j=0; j<mWidth; j++)
				{
					unsigned short aColor = *src++;
					*dest++ = SURFACE_555_TO_32(aColor);
				}
				row += mSurface->GetBytesPerRow();
			}
			break;
		}

		case SurfaceFormat_565:
		{
			unsigned char *row = (unsigned char*)mSurface->GetPixelsBase();
			DWORD *dest = anImage->GetImageData();
			int aNumPixels = mSurface->GetWidth()*mSurface->GetHeight();
			for(int i=0; i<mHeight; i++)
			{
				unsigned short *src = (unsigned short*)row;
				for(int j=0; j<mWidth; j++)
				{
					unsigned short aColor = *src++;
					*dest++ = SURFACE_565_TO_32(aColor);
				}
				row += mSurface->GetBytesPerRow();
			}
			break;
		}

		case SurfaceFormat_32:	
			memcpy(anImage->GetImageData(),mSurface->GetPixelsBase(),mWidth*mHeight*4); 
			break;
	}

	DWORD *aPtr = anImage->GetImageData();
	if(mTransparentMask.get()!=NULL)
	{
		unsigned char *aMaskRow = (unsigned char*)((SNativeMask*)mTransparentMask.get())->GetData();
		unsigned char aMask;
		unsigned char *aMaskPtr;
		int aMaskBytesPerRow = mWidth/8;
		if(mWidth%8!=0)
			aMaskBytesPerRow++;


		anImage->SetDoTransparency(true);
		
		anImage->StartRowTraversal(0);
		for(int i=0; i<mHeight; i++)
		{
			aMask = 128;
			aMaskPtr = aMaskRow;
			while(anImage->HasMoreRowPixels())
			{
				if(!((*aMaskPtr) & aMask))
					anImage->SetRowPixel(0xff000000 | anImage->GetRowPixel());

				anImage->NextRowPixel();
				aMask>>=1;
				if(aMask==0)
				{
					aMask = 128;
					aMaskPtr++;
				}
			}
			anImage->NextRow();
			aMaskRow += aMaskBytesPerRow;
		}
	}
	return anImage.get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr SNativeImage::Duplicate()
{
	return new SNativeImage(mSurface->Duplicate());
}



