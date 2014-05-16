#include "ImageFilter.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiImageFilter* MultiImageFilter::Add(ImageFilter *theFilter)
{
	mFilterList.push_back(theFilter);
	return this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr MultiImageFilter::Filter(RawImage *theImage) 
{ 
	FilterList::iterator anItr = mFilterList.begin();
	RawImagePtr anImage = theImage;
	while(anItr!=mFilterList.end())
	{
		ImageFilter *aFilter = *anItr;
		anImage = aFilter->Filter(anImage);
		++anItr;
	}
	return anImage; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void ImageFilter_NewRawImage(RawImage1Ptr &theImage, int theWidth, int theHeight) { theImage = Window::CreateRawImage1(theWidth,theHeight); }
static void ImageFilter_NewRawImage(RawImage8Ptr &theImage, int theWidth, int theHeight) { theImage = Window::CreateRawImage8(theWidth,theHeight); }
static void ImageFilter_NewRawImage(RawImage32Ptr &theImage, int theWidth, int theHeight) { theImage = Window::CreateRawImage32(theWidth,theHeight); }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static SmartPtr<ImageType> DoCrop(ImageType *theImage, int x, int y, int width, int height)
{
	SmartPtr<ImageType> anImage; //= new ImageType(width,height);
	ImageFilter_NewRawImage(anImage,width,height); 
	anImage->CopyInfo(theImage);

	for(int i=0; i<height; i++)
	{
		PixelType *aFromPixel = theImage->StartRowTraversal(y+i) + x;
		PixelType *aToPixel = anImage->StartRowTraversal(i);
		memcpy(aToPixel,aFromPixel,width*sizeof(PixelType));
	}	

	return anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr CropImageFilter::Filter(RawImage *theImage)
{
	switch(theImage->GetType())
	{
		case RawImageType_8: return DoCrop<RawImage8,RawImage8::PixelType>((RawImage8*)theImage,mx,my,mWidth,mHeight).get(); 
		case RawImageType_32: return DoCrop<RawImage32,RawImage32::PixelType>((RawImage32*)theImage,mx,my,mWidth,mHeight).get(); 
		default: return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TrimFilter_IsRowEmpty(unsigned char *pixels, unsigned char transparentPixel, int width)
{
	for(int i=0; i<width; i++)
		if(*pixels++!=transparentPixel)
			return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TrimFilter_IsRowEmpty(DWORD *pixels, int width)
{
	for(int i=0; i<width; i++)
		if((*pixels++)&0xff000000)
			return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TrimFilter_IsColEmpty(unsigned char *pixels, unsigned char transparentPixel, int height, int rowInc)
{
	for(int i=0; i<height; i++, pixels+=rowInc)
	{
		if(*pixels!=transparentPixel)
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TrimFilter_IsColEmpty(DWORD *pixels, int height, int rowInc)
{
	for(int i=0; i<height; i++, pixels+=rowInc)
	{
		if((*pixels)&0xff000000)
			return false;
	}

	return true;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr TrimImageFilter::Filter(RawImage *theImage)
{
	mx = my = 0;
	mWidth = theImage->GetWidth();
	mHeight = theImage->GetHeight();

	if(!theImage->HasTransparency())
		return theImage;

	int x,y;
	int left = 0,top = 0,right = mWidth-1, bottom = mHeight-1;
	if(theImage->GetType()==RawImageType_8)
	{
		RawImage8 *anImage = (RawImage8*)theImage;
		unsigned char transparentPixel = anImage->GetTransparentColor();
		unsigned char *pixels = anImage->GetImageData();
		int bytesPerRow = anImage->GetBytesPerRow();

		// Find Top
		for(y=0; y<mHeight; y++, pixels+=bytesPerRow)
		{
			if(!TrimFilter_IsRowEmpty(pixels,transparentPixel,mWidth))
				break;
		}
		if(y>=mHeight)
			return NULL;

		top = y;

		// Find Bottom
		pixels = anImage->GetImageData() + (mHeight-1)*bytesPerRow;
		for(y=bottom; y>top; y--, pixels-=bytesPerRow)
		{
			if(!TrimFilter_IsRowEmpty(pixels,transparentPixel,mWidth))
				break;
		}
		bottom = y;
		mHeight = bottom-top+1;

		// Find Left
		pixels = anImage->GetImageData() + top*bytesPerRow;
		for(x=0; x<mWidth; x++, pixels++)
		{
			if(!TrimFilter_IsColEmpty(pixels,transparentPixel,mHeight,bytesPerRow))
				break;
		}
		if(x>=mWidth)
			return NULL;

		left = x;
		
		// Find Right
		pixels = anImage->GetImageData() + top*bytesPerRow + (mWidth-1);
		for(x=right; x>left; x--, pixels--)
		{
			if(!TrimFilter_IsColEmpty(pixels,transparentPixel,mHeight,bytesPerRow))
				break;
		}
		right = x;
		mWidth = right-left+1;
	}
	else if(theImage->GetType()==RawImageType_32)
	{
		RawImage32 *anImage = (RawImage32*)theImage;
		DWORD *pixels = anImage->GetImageData();
		int bytesPerRow = mWidth;

		// Find Top
		for(y=0; y<mHeight; y++, pixels+=bytesPerRow)
		{
			if(!TrimFilter_IsRowEmpty(pixels,mWidth))
				break;
		}
		if(y>=mHeight)
			return NULL;

		top = y;

		// Find Bottom
		pixels = anImage->GetImageData() + (mHeight-1)*bytesPerRow;
		for(y=bottom; y>top; y--, pixels-=bytesPerRow)
		{
			if(!TrimFilter_IsRowEmpty(pixels,mWidth))
				break;
		}
		bottom = y;
		mHeight = bottom-top+1;

		// Find Left
		pixels = anImage->GetImageData() + top*bytesPerRow;
		for(x=0; x<mWidth; x++, pixels++)
		{
			if(!TrimFilter_IsColEmpty(pixels,mHeight,bytesPerRow))
				break;
		}
		if(x>=mWidth)
			return NULL;

		left = x;
		
		// Find Right
		pixels = anImage->GetImageData() + top*bytesPerRow + (mWidth-1);
		for(x=right; x>left; x--, pixels--)
		{
			if(!TrimFilter_IsColEmpty(pixels,mHeight,bytesPerRow))
				break;
		}
		right = x;
		mWidth = right-left+1;
	}
	else
		return NULL;

	mx = left; my = top;
	mWidth = right-left+1;
	mHeight = bottom-top+1;
	return CropImageFilter::Filter(theImage);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScaleImageFilter::SetPercentSize(int thePercentWidth, int thePercentHeight)
{
	mPercentWidth = thePercentWidth;
	mPercentHeight = thePercentHeight;
	mIsPercent = true;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScaleImageFilter::SetAbsSize(int theWidth, int theHeight)
{
	mWidth = theWidth;
	mHeight = theHeight;
	mIsPercent = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScaleImageFilter::CheckSize(Image *theOldImage)
{
	if(mIsPercent)
	{
		mWidth = theOldImage->GetWidth()*mPercentWidth/100;
		mHeight = theOldImage->GetHeight()*mPercentHeight/100;
	}

	if(mWidth<=0)
		mWidth = 1;
	if(mHeight<=0)
		mHeight = 1;

	if(mWidth==theOldImage->GetWidth() && mHeight==theOldImage->GetHeight())
		return true;
	else 
		return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static SmartPtr<ImageType> DoScale(ImageType *theImage, int theNewWidth, int theNewHeight)
{
	SmartPtr<ImageType> anImage;// = new ImageType(theNewWidth,theNewHeight);
	ImageFilter_NewRawImage(anImage,theNewWidth,theNewHeight);
	anImage->CopyInfo(theImage);

	int cx = 0, cy = 0, oy = 0;
	for(int y=0; y<theNewHeight; y++)
	{
		theImage->StartRowTraversal(oy);
		anImage->StartRowTraversal(y);
		PixelType curVal = theImage->GetRowPixel();

		int cx = 0;
		for(int x=0; x<theNewWidth; x++)
		{
			anImage->SetRowPixel(curVal);
			anImage->NextRowPixel();
			cx += theImage->GetWidth();
			if(cx >= theNewWidth)
			{
				theImage->NextRowPixel(cx/theNewWidth);
				cx%=theNewWidth;
				curVal = theImage->GetRowPixel();
			}
		}

		cy += theImage->GetHeight();
		if(cy >= theNewHeight)
		{
			oy += cy/theNewHeight;
			cy%=theNewHeight;
		}
	}

	return anImage;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr ScaleImageFilter::Filter(RawImage *theImage)
{
	if(CheckSize(theImage))
		return theImage;

	switch(theImage->GetType())
	{
	case RawImageType_1: return DoScale<RawImage1,RawImage1::PixelType>((RawImage1*)theImage,mWidth,mHeight).get(); break;
	case RawImageType_8: return DoScale<RawImage8,RawImage8::PixelType>((RawImage8*)theImage,mWidth,mHeight).get(); break;
	case RawImageType_32: return DoScale<RawImage32,RawImage32::PixelType>((RawImage32*)theImage,mWidth,mHeight).get(); break;
	}

	return theImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr TintImageFilter::Filter(RawImage *theImage)
{
	if(theImage->GetType()==RawImageType_1)
		return theImage;

	if(theImage->GetType()==RawImageType_8)
	{
		RawImage8 *anImage = (RawImage8*)theImage;
		PalettePtr aPalette = anImage->GetPalette()->Duplicate();
		for(int i=0; i<aPalette->GetNumColors(); i++)
		{
			DWORD aColor = aPalette->GetColor(i);
			int r = ((aColor&0xff0000)>>16)*mRed/100;
			int g = ((aColor&0xff00)>>8)*mGreen/100;
			int b = ((aColor&0xff)*mBlue)/100;
			if(r>255) r=255;
			if(g>255) g=255;
			if(b>255) b=255;
			aPalette->SetColor(i,(r<<16) | (g<<8) | b);
		}
		anImage->SetPalette(aPalette);
		return anImage;
	}
	else if(theImage->GetType()==RawImageType_32)
	{
		int aWidth = theImage->GetWidth();
		int aHeight = theImage->GetHeight();
		RawImage32 *anImage = (RawImage32*)theImage;
		anImage->StartRowTraversal(0);
		while(anImage->HasMoreRows())
		{
			while(anImage->HasMoreRowPixels())
			{
				DWORD aColor = anImage->GetRowPixel();
				int r = ((aColor&0xff0000)>>16)*mRed/100;
				int g = ((aColor&0xff00)>>8)*mGreen/100;
				int b = ((aColor&0xff)*mBlue)/100;
				if(r>255) r=255;
				if(g>255) g=255;
				if(b>255) b=255;

				anImage->SetRowPixel(aColor&0xff000000 | (r<<16) | (g<<8) | b);
				anImage->NextRowPixel();
			}
			anImage->NextRow();
		}

		return anImage;
	}

	return theImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr TransparentImageFilter::Filter(RawImage *theImage)
{
	if(theImage->GetType()==RawImageType_1)
		return theImage;

	DWORD aTransparentColor = mTransparentColor;
	if(mUseSample)
	{
		if(theImage->GetHeight()<=0 || theImage->GetWidth()<=0)
			return theImage;

		int x = 0, y = 0;
		switch(mPixelSample)
		{
			case UpperLeft: x = 0; y = 0; break;
			case UpperRight: x = theImage->GetWidth()-1; y = 0; break;
			case LowerLeft: x = 0; y = theImage->GetHeight()-1; break;
			case LowerRight: x = theImage->GetWidth()-1; y = theImage->GetHeight()-1; break;
		}

		if(theImage->GetType()==RawImageType_8)
		{
			RawImage8 *anImage = (RawImage8*)theImage;
			unsigned char aPixel = anImage->GetPixel(x,y);
			anImage->SetTransparentColor(aPixel);
			return anImage;
		}
		else if(theImage->GetType()==RawImageType_32)
		{
			RawImage32 *anImage = (RawImage32*)theImage;
			aTransparentColor = anImage->GetPixel(x,y);
		}
	}
		

	if(theImage->GetType()==RawImageType_8)
	{
		RawImage8 *anImage = (RawImage8*)theImage;
		Palette* aPalette = anImage->GetPalette();
		for(int i=0; i<aPalette->GetNumColors(); i++)
		{
			DWORD aColor = aPalette->GetColor(i);
			if(aColor==aTransparentColor)
			{
				anImage->SetTransparentColor(i);
				break;
			}
		}
		return anImage;
	}
	else if(theImage->GetType()==RawImageType_32)
	{
		int aWidth = theImage->GetWidth();
		int aHeight = theImage->GetHeight();
		RawImage32 *anImage = (RawImage32*)theImage;
		anImage->SetDoTransparency(true);
		anImage->StartRowTraversal(0);
		while(anImage->HasMoreRows())
		{
			while(anImage->HasMoreRowPixels())
			{
				DWORD aColor = anImage->GetRowPixel();
				if(aColor==aTransparentColor)
					anImage->SetRowPixel(0);
				else
					anImage->SetRowPixel(0xff000000 | aColor);

				anImage->NextRowPixel();
			}
			anImage->NextRow();
		}
		return anImage;
	}

	return theImage;
}

