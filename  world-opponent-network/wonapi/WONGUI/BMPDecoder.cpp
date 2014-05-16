#include "WONCommon/Platform.h"

#include "BMPDecoder.h"
#include "WONCommon/FileReader.h"
#include "NativeImage.h"
#include "DisplayContext.h"
#include "RawImage.h"

using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool BMPReadFileHeader(FileReader &theReader)
{
	unsigned short bfType = theReader.ReadShort();
	unsigned long bfSize = theReader.ReadLong();
	unsigned short bfReserved1 = theReader.ReadShort(); 
	unsigned short bfReserved2 = theReader.ReadShort(); 
	unsigned long bfOffBits = theReader.ReadLong(); 

	if (bfType != ((unsigned short) ('M' << 8) | 'B'))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BMPDecoder::DoGetAttrib(ImageAttributes &theAttrib)
{
	if(!BMPReadFileHeader(*mFileReader))
		return false;

	int aPos = mFileReader->pos();

	DWORD  biSize = mFileReader->ReadLong(); 
	long biWidth = mFileReader->ReadLong(); 
	long biHeight = mFileReader->ReadLong(); 
	theAttrib.mWidth = biWidth;
	theAttrib.mHeight = biHeight;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr BMPDecoder::DoDecodeRaw()
{
	try
	{
		if(!BMPReadFileHeader(*mFileReader))
			return NULL;

		int aPos = mFileReader->pos();

		DWORD  biSize = mFileReader->ReadLong(); 
		long biWidth = mFileReader->ReadLong(); 
		long biHeight = mFileReader->ReadLong(); 
		short biPlanes = mFileReader->ReadShort(); 
		short biBitCount = mFileReader->ReadShort();
		DWORD  biCompression = mFileReader->ReadLong(); 
		DWORD  biSizeImage = mFileReader->ReadLong(); 
		long biXPelsPerMeter = mFileReader->ReadLong(); 
		long biYPelsPerMeter = mFileReader->ReadLong(); 
		DWORD  biClrUsed = mFileReader->ReadLong(); 
		DWORD  biClrImportant = mFileReader->ReadLong(); 

		mFileReader->SkipBytes(biSize - (mFileReader->pos() - aPos));

		int aNumColors = biClrUsed?biClrUsed:(1<<biBitCount);
		int aTransparentIndex = -1;
		PalettePtr aPalette;
		if(biBitCount<=8)
		{
			aPalette = new Palette(aNumColors);
			for(int i=0; i<aNumColors; i++)
			{
				unsigned char r,g,b,a;
				b = mFileReader->ReadByte();
				g = mFileReader->ReadByte();
				r = mFileReader->ReadByte();
				a = mFileReader->ReadByte();

				DWORD aColor = (r<<16) | (g<<8) | b;
//				if(aColor==mTransparentColor)
//					aTransparentIndex = i; 
				
				aPalette->SetColor(i, aColor);
			}
		}

		int x,y;
		int startY, dy, yact;
		if(biHeight<0)
		{
			biHeight = -biHeight;
			startY = 0;
			dy = 1;
		}
		else
		{
			startY = biHeight-1;
			dy = -1;
		}


		if(biBitCount==4)
		{
			int anExtra = (biWidth%8);
			if(anExtra!=0)
			{
				anExtra = 8 - anExtra;
				anExtra/=2;
			}

			RawImage8Ptr anImage = CreateRawImage8(biWidth,biHeight,aPalette);
			if(aTransparentIndex>=0)
				anImage->SetTransparentColor(aTransparentIndex);

			for(y=0, yact=startY; y<biHeight; y++, yact+=dy)
			{
				anImage->StartRowTraversal(yact);
				for(x=0; x<biWidth; x++)
				{
					unsigned char aByte = mFileReader->ReadByte();
					anImage->SetRowPixel((aByte>>4)&0x0f);
					x++;
					if(x<biWidth)
					{
						anImage->NextRowPixel();
						anImage->SetRowPixel((aByte)&0x0f);
					}
					anImage->NextRowPixel();
				}

				for(x=0; x<anExtra; x++)
					mFileReader->ReadByte();
			}

			return anImage.get();
		}
		if(biBitCount==8)
		{
			int anExtra = biWidth%4;
			if(anExtra!=0)
				anExtra = 4 - anExtra;

			RawImage8Ptr anImage = CreateRawImage8(biWidth,biHeight,aPalette);
			if(aTransparentIndex>=0)
				anImage->SetTransparentColor(aTransparentIndex);

			for(y=0, yact=startY; y<biHeight; y++, yact+=dy)
			{
				anImage->StartRowTraversal(yact);
				for(x=0; x<biWidth; x++)
				{
					anImage->SetRowPixel(mFileReader->ReadByte());
					anImage->NextRowPixel();
				}

				for(x=0; x<anExtra; x++)
					mFileReader->ReadByte();
			}

			return anImage.get();
		}
		else if(biBitCount==24)
		{
			int anExtra = (biWidth*3)%4;
			if(anExtra!=0)
				anExtra = 4 - anExtra;

			RawImage32Ptr anImage = CreateRawImage32(biWidth,biHeight);
//			if(mTransparentColor>=0)
//				anImage->SetDoTransparency(true);

			for(y=0, yact=startY; y<biHeight; y++, yact+=dy)
			{
				anImage->StartRowTraversal(yact);
				for(x=0; x<biWidth; x++)
				{
					int aPixel = mFileReader->ReadByte() | (mFileReader->ReadByte()<<8) | (mFileReader->ReadByte()<<16);
//					if(aPixel!=mTransparentColor)
//						aPixel |= 0xff000000;

					anImage->SetRowPixel(aPixel);
					anImage->NextRowPixel();
				}

				for(x=0; x<anExtra; x++)
					mFileReader->ReadByte();
			}

			return anImage.get();
		}
	}
	catch(FileReaderException&)
	{
	}

	return NULL;
}

