#include "TGADecoder.h"

using namespace WONAPI;

namespace
{

struct TGAHeader 
{
	char  idlength;
	char  colormaptype;
	char  datatypecode;
	short int colormaporigin;
	short int colormaplength;
	char  colormapdepth;
	short int x_origin;
	short int y_origin;
	short width;
	short height;
	char  bitsperpixel;
	char  imagedescriptor;
};

#define TGA_16_TO_32(x)	((((x)&0x1f)<<3) | ((((x)>>5)&0x1f)<<11) | ((((x)>>10)&0x1f)<<19) | (((x)&0x8000)?0xff:00))

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static TGAHeader TGA_ReadHeader(FileReader &theReader)
{
	TGAHeader aHeader;

	aHeader.idlength = theReader.ReadByte();
	aHeader.colormaptype = theReader.ReadByte();
	aHeader.datatypecode = theReader.ReadByte();
	aHeader.colormaporigin = theReader.ReadShort();
	aHeader.colormaplength = theReader.ReadShort();
	aHeader.colormapdepth = theReader.ReadByte();
	aHeader.x_origin = theReader.ReadShort();
	aHeader.y_origin = theReader.ReadShort();
	aHeader.width = theReader.ReadShort();
	aHeader.height = theReader.ReadShort();
	aHeader.bitsperpixel = theReader.ReadByte();
	aHeader.imagedescriptor = theReader.ReadByte();

	int bpp = aHeader.bitsperpixel;
	int ctype = aHeader.colormaptype;
	int datatype = aHeader.datatypecode;
	if(datatype!=1 && datatype!=2 && datatype!=9 && datatype!=10)
		throw FileReaderException();
	if(ctype!=0 && ctype!=1)
		throw FileReaderException();

	if(ctype==1)
	{
		if(aHeader.colormaplength>256)
			throw FileReaderException();

		char cmd = aHeader.colormapdepth;
		if(cmd!=16 && cmd!=24 && cmd!=32)
			throw FileReaderException();

		if(bpp>8)
			throw FileReaderException();
	}
	else if(bpp<16)
		throw FileReaderException();

	if(bpp!=8 && bpp!=16 && bpp!=24 && bpp!=32)
		throw FileReaderException();
		
	theReader.SkipBytes(aHeader.idlength); // skip comment

	return aHeader;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static DWORD* TGA_ReadColorMap(const TGAHeader &theHeader, FileReader &theReader)
{
	if(theHeader.colormaplength<=0)
		return NULL;

	DWORD *aPalette = new DWORD[256];
	std::auto_ptr<DWORD> aDelPalette(aPalette);

	int num = theHeader.colormaplength;
	int i;
	if(theHeader.colormapdepth==32)
	{
		for(i=0; i<num; i++)
			aPalette[i] = theReader.ReadLong();
	}
	else if(theHeader.colormapdepth==24)
	{
		for(i=0; i<num; i++)
		{
			int b = theReader.ReadByte();
			int g = theReader.ReadByte();
			int r = theReader.ReadByte();
			int a = 0xff;

			aPalette[i] = (a<<24) | (r<<16) | (g<<8) | b;
		}
	}
	else if(theHeader.colormapdepth==16) // 16-bit
	{
		for(i=0; i<num; i++)
		{
			unsigned short pixel = theReader.ReadShort(); // ARRRRRGG GGGBBBBB
			aPalette[i] = TGA_16_TO_32(pixel);
		}
	}
	else 
		return NULL;

	for(; i<256; i++)
		aPalette[i] = 0;

	return aDelPalette.release();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void TGA_FlipImage(RawImage32 *theImage)
{
	int i;

	int width = theImage->GetWidth();
	int height = theImage->GetHeight();

	DWORD *topRow = theImage->GetImageData();
	DWORD *bottomRow = topRow + (theImage->GetHeight()-1)*theImage->GetWidth();
	while(topRow<bottomRow)
	{
		DWORD *p1 = topRow;
		DWORD *p2 = bottomRow;
		for(i=0; i<width; i++)
		{
			DWORD temp = *p1;
			*p1++ = *p2;
			*p2++ = temp;
		}

		topRow+=width;
		bottomRow-=width;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TGA_DecodeUncompressedImage(FileReader &theReader, RawImage32 *theImage, DWORD *thePalette, TGAHeader &theHeader)
{
	DWORD *pixels = theImage->GetImageData();
	int width = theHeader.width;
	int height = theHeader.height;
	int bpp = theHeader.bitsperpixel;

	int i,j;
	if(bpp==8)
	{
		if(thePalette==NULL)
			return false;

		for(i=0; i<height; i++)
			for(j=0; j<width; j++)
				*pixels++ = thePalette[theReader.ReadByte()];			
	}
	else if(bpp==16)
	{
		for(i=0; i<height; i++)
		{
			for(j=0; j<width; j++)
			{
				unsigned short pixel = theReader.ReadShort();
				*pixels++ = TGA_16_TO_32(pixel);
			}
		}
	}
	else if(bpp==24)
	{
		for(i=0; i<height; i++)
		{
			for(j=0; j<width; j++)
			{
				int b = theReader.ReadByte();
				int g = theReader.ReadByte();
				int r = theReader.ReadByte();
				*pixels++ = (0xff000000) | (r<<16) | (g<<8) | b;
			}
		}
	}
	else if(bpp=32)
	{
		for(i=0; i<height; i++)
			for(j=0; j<width; j++)
				*pixels++ = theReader.ReadLong();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TGA_DecodeCompressedImage(FileReader &theReader, RawImage32 *theImage, DWORD *thePalette, TGAHeader &theHeader)
{
	DWORD *pixels = theImage->GetImageData();
	int width = theHeader.width;
	int height = theHeader.height;
	int bpp = theHeader.bitsperpixel;

	DWORD pixelList[256];
	int i,j,k, numPixels = 0;
	if(bpp==8)
	{
		if(thePalette==NULL)
			return false;

		for(i=0; i<height; i++)
		{
			for(j=0; j<width; j++)
			{
				while(numPixels<=0)
				{
					unsigned char rleHeader = theReader.ReadByte();
					numPixels = (rleHeader&0x7f)+1;
					if(rleHeader&0x80) // 
					{
						DWORD aColor = thePalette[theReader.ReadByte()];
						for(k=numPixels-1; k>=0; k--)
							pixelList[k] = aColor;
					}
					else
					{
						for(k=numPixels-1; k>=0; k--)
							pixelList[k] = thePalette[theReader.ReadByte()];
					}
		
					if(numPixels==0)
						return NULL;
				}
	
				*pixels++ = pixelList[numPixels--];
			}
		}
	}
	else if(bpp==16)
	{
		for(i=0; i<height; i++)
		{
			for(j=0; j<width; j++)
			{
				while(numPixels<=0)
				{
					unsigned char rleHeader = theReader.ReadByte();
					numPixels = (rleHeader&0x7f)+1;
					if(rleHeader&0x80) // 
					{
						unsigned short aPixel = theReader.ReadShort();
						DWORD aColor = TGA_16_TO_32(aPixel);
						for(k=numPixels-1; k>=0; k--)
							pixelList[k] = aColor;
					}
					else
					{
						for(k=numPixels-1; k>=0; k--)
						{
							unsigned short aPixel = theReader.ReadShort();
							pixelList[k] = aPixel;
						}
					}
				}
					
				*pixels++ = pixelList[numPixels--];
			}
		}

	}
	else if(bpp==24)
	{
		for(i=0; i<height; i++)
		{
			while(numPixels<=0)
			{
				unsigned char rleHeader = theReader.ReadByte();
				numPixels = (rleHeader&0x7f)+1;
				if(rleHeader&0x80) // 
				{
					int b = theReader.ReadByte();
					int g = theReader.ReadByte();
					int r = theReader.ReadByte();
					DWORD aColor = (0xff000000) | (r<<16) | (g<<8) | b;
					for(k=numPixels-1; k>=0; k--)
						pixelList[k] = aColor;
				}
				else
				{
					for(k=numPixels-1; k>=0; k--)
					{
						int b = theReader.ReadByte();
						int g = theReader.ReadByte();
						int r = theReader.ReadByte();
						pixelList[k] = (0xff000000) | (r<<16) | (g<<8) | b;
					}
				}
			}
				
			*pixels++ = pixelList[numPixels--];
		}
	}
	else if(bpp=32)
	{
		for(i=0; i<height; i++)
		{
			for(j=0; j<width; j++)
			{
				while(numPixels<=0)
				{
					unsigned char rleHeader = theReader.ReadByte();
					numPixels = (rleHeader&0x7f)+1;
					if(rleHeader&0x80) // 
					{
						DWORD aColor = theReader.ReadLong();
						for(k=numPixels-1; k>=0; k--)
							pixelList[k] = aColor;
					}
					else
					{
						for(k=numPixels-1; k>=0; k--)
							pixelList[k] = theReader.ReadLong();
					}
				}
					
				*pixels++ = pixelList[--numPixels];			
			}
		}
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TGA_CheckTransparency(FileReader &theReader, RawImage32 *theImage, TGAHeader &theHeader)
{
	DWORD *pixels = theImage->GetImageData();
	int width = theHeader.width;
	int height = theHeader.height;

	int i,j;
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			if(*pixels++ & 0x80000000)
			{
				theImage->SetDoTransparency(true);
				return true;
			}
		}
	}

	return false;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr TGADecoder::DoDecodeRaw()
{
	TGAHeader aHeader = TGA_ReadHeader(*mFileReader);
	DWORD *palette = NULL;

	int paletteSize = aHeader.colormaplength;
	int datatype = aHeader.datatypecode;
	bool hasPalette = aHeader.colormaptype!=0;

	if(hasPalette)
	{
		palette = TGA_ReadColorMap(aHeader,*mFileReader);
		if(palette==NULL)
			return NULL;
	}

	std::auto_ptr<DWORD> aDelPalette(palette);

	RawImage32Ptr anImage = CreateRawImage32(aHeader.width,aHeader.height);

	if(datatype==1 || datatype==2)
	{
		if(!TGA_DecodeUncompressedImage(*mFileReader,anImage,palette,aHeader))
			return NULL;
	}
	else if(datatype==9 || datatype==10)
	{
		if(!TGA_DecodeCompressedImage(*mFileReader,anImage,palette,aHeader))
			return NULL;
	}
	else
		return NULL;

	TGA_CheckTransparency(*mFileReader,anImage,aHeader);

	bool upsideDown = !((aHeader.imagedescriptor >> 5) & 0x01);
	if(upsideDown)
		TGA_FlipImage(anImage);


	return anImage.get();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TGADecoder::DoGetAttrib(ImageAttributes &theAttrib)
{
	TGAHeader aHeader = TGA_ReadHeader(*mFileReader);
	theAttrib.mWidth = aHeader.width;
	theAttrib.mHeight = aHeader.height;
	return true;
}
