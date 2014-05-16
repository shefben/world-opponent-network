#include "SRawImage.h"
#include "SNativeImage.h"
#include "SDisplayContext.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SRawImage32::SRawImage32(int theWidth, int theHeight) : RawImage32(theWidth, theHeight)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr SRawImage32::PlatformGetNative(DisplayContext *theContext)
{
	SurfaceBasePtr aSurface = SurfaceBase::GetNewDefaultSurface();
	aSurface->Create(mWidth,mHeight);
	switch(aSurface->GetSurfaceFormat())
	{
		case SurfaceFormat_555:
		{
			DWORD *src = GetImageData();
			unsigned char *row = (unsigned char*)aSurface->GetPixelsBase();
			for(int i=0; i<mHeight; i++)
			{
				unsigned short *dest = (unsigned short*)row;
				for(int j=0; j<mWidth; j++)
				{
					DWORD aColor = *src++;
					*dest++ = SURFACE_32_TO_555(aColor);
				}

				row += aSurface->GetBytesPerRow();
			}
			break;
		}

		case SurfaceFormat_565:
		{
			DWORD *src = GetImageData();
			unsigned char *row = (unsigned char*)aSurface->GetPixelsBase();
			for(int i=0; i<mHeight; i++)
			{
				unsigned short *dest = (unsigned short*)row;
				for(int j=0; j<mWidth; j++)
				{
					DWORD aColor = *src++;
					*dest++ = SURFACE_32_TO_565(aColor);
				}

				row += aSurface->GetBytesPerRow();
			}
			break;
		}

		case SurfaceFormat_32: 
			memcpy(aSurface->GetPixelsBase(), mImageData, mWidth*mHeight*4); 
			break;

	}


	return new SNativeImage(aSurface);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImage32* SRawImage32::PlatformDuplicate()
{
	return new SRawImage32(mWidth,mHeight);
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SRawImage8::SRawImage8(int theWidth, int theHeight, Palette *thePalette) : RawImage8(theWidth,theHeight,thePalette)
{
	mBytesPerRow = mWidth;

	FinishConstructor();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImage8* SRawImage8::PlatformDuplicate()
{
	return new SRawImage8(mWidth,mHeight,mPalette);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr SRawImage8::PlatformGetNative(DisplayContext *theContext)
{
	SurfaceBasePtr aSurface = SurfaceBase::GetNewDefaultSurface();
	aSurface->Create(mWidth,mHeight);

	switch(aSurface->GetSurfaceFormat())
	{
		case SurfaceFormat_555:
		{
			unsigned char *aRowPtr = (unsigned char*)aSurface->GetPixelsBase();
			StartRowTraversal(0);
			for(int i=0; i<mHeight; i++)
			{
				unsigned short *dest = (unsigned short*)aRowPtr;
				for(int j=0; j<mWidth; j++)
				{
					unsigned char aPixel = GetRowPixel();
					DWORD aColor = mPalette->GetColor(aPixel);
					*dest++ = SURFACE_32_TO_555(aColor);
					NextRowPixel();
				}
				NextRow();
				aRowPtr += aSurface->GetBytesPerRow();
			}
			break;
		}

		case SurfaceFormat_565:
		{
			unsigned char *aRowPtr = (unsigned char*)aSurface->GetPixelsBase();
			StartRowTraversal(0);
			for(int i=0; i<mHeight; i++)
			{
				unsigned short *dest = (unsigned short*)aRowPtr;
				for(int j=0; j<mWidth; j++)
				{
					unsigned char aPixel = GetRowPixel();
					DWORD aColor = mPalette->GetColor(aPixel);
					*dest++ = SURFACE_32_TO_565(aColor);
					NextRowPixel();
				}
				NextRow();
				aRowPtr += aSurface->GetBytesPerRow();
			}
			break;
		}
	
		case SurfaceFormat_32:
		{
			DWORD *aRowPtr = (DWORD*)aSurface->GetPixelsBase();
			StartRowTraversal(0);
			for(int i=0; i<mHeight; i++)
			{
				for(int j=0; j<mWidth; j++)
				{
					unsigned char aPixel = GetRowPixel();
					*aRowPtr++ = mPalette->GetColor(aPixel);
					NextRowPixel();
				}
				NextRow();
			}
			break;
		}

	}


	return new SNativeImage(aSurface);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SRawImage1::SRawImage1(int theWidth, int theHeight) : RawImage1(theWidth, theHeight)
{
	mBytesPerRow = mBytesPerRowMinusPad;

	FinishConstructor();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeMaskPtr SRawImage1::GetMask(DisplayContext *theContext)
{
	char *aMem = new char[mHeight*mBytesPerRow];
	memcpy(aMem,mImageData,mHeight*mBytesPerRow);

	return new SNativeMask(theContext, new ByteBuffer(aMem,mHeight*mBytesPerRow));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr SRawImage1::GetNative(DisplayContext *theContext)
{
	return NULL;
}

