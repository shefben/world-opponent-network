#include "SFont.h"
#include "SDisplayContext.h"

#include "WONCommon/FileReader.h"
#include "WONCommon/ReadBuffer.h"

using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFont::SFont(const FontDescriptor &theDescriptor) : Font(NULL, theDescriptor)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SFont::Read(const char *theFilePath)
{
	FileReader aReader(theFilePath);
	if(!aReader.IsOpen())
		return false;

	try
	{
		char aSig[5];
		aReader.ReadBytes(aSig,4);
		aSig[4] = '\0';
		if(strcmp(aSig,"FONT")!=0)
			return false;

		if(aReader.ReadLong()!=1)
			return false;

		int aTableOffset = aReader.ReadLong();
		if(aTableOffset<=aReader.pos())
			return false;

		int aNumBytes = aTableOffset - aReader.pos();
		if(aNumBytes<=0)
			return false;

		int anOffset = aReader.pos();
		char *aBuf = new char[aNumBytes];
		ByteBufferPtr aFontData = new ByteBuffer(aBuf,aNumBytes,true);
		aReader.ReadBytes(aBuf,aNumBytes);

		mHeight = aReader.ReadShort();
		mAscent = aReader.ReadShort();
		mDescent = aReader.ReadShort();
		for(int i=32; i<255; i++)
		{
			mCharWidth[i] = aReader.ReadShort();
			mCharData[i] = aBuf + (aReader.ReadLong()-anOffset);
		}

		mFontData = aFontData;
	}
	catch(FileReaderException&)
	{
		return false;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void* SFont::GetCharData(int theChar)
{
	return mCharData[theChar];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void* SFont::GetCharData(int theChar, int *theWidth)
{
	if(theChar<32 || theChar>=255)
		theChar = 127;

	*theWidth = mCharWidth[theChar];
	return mCharData[theChar];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
static void Surface_DrawString(Surface<PixelType> *theSurface, SFont *theFont, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	int mClipX1, mClipX2, mClipY1, mClipY2;
	theSurface->GetClipRect(mClipX1,mClipY1,mClipX2,mClipY2);
	if(x>mClipX2 || y>mClipY2 || y+theFont->GetHeight()<=mClipY1)
		return;

	ReadBuffer aBuf;
	
	int aHeight = theFont->GetHeight();
	int mPixelsPerRow = theSurface->GetPixelsPerRow();
	PixelType *anUpperLeftCorner = theSurface->GetPixels() + y*mPixelsPerRow + x;
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
						theSurface->PutPixel(x+j,y+i,aPixel);

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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SFont::DrawString(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	SurfaceBase *aSurface = ((SDisplayContext*)theContext)->mSurface;
	switch(aSurface->GetSurfaceFormat())
	{
		case SurfaceFormat_555:
		case SurfaceFormat_565:
			Surface_DrawString<unsigned short>((Surface16*)aSurface,this,theStr,theOffset,theLength,x,y);
			break;
		
		case SurfaceFormat_32:
			Surface_DrawString<DWORD>((Surface32*)aSurface,this,theStr,theOffset,theLength,x,y);
			break;
	}

//	aSurface->DrawString(this,theStr,theOffset,theLength,x,y);

	if(GetStyle() & FontStyle_Underline)
		DrawUnderline(theContext,theStr,theOffset,theLength,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int SFont::GetStringWidth(const GUIString &theStr, unsigned short theOffset, unsigned short theLength)
{
	int aWidth = 0;
	for(int i=0; i<theLength; i++)
	{
		int aChar = theStr.at(theOffset+i);
		if(aChar<32 || aChar>=255)
			aChar = 127;

		aWidth += mCharWidth[aChar];
	}

	return aWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int SFont::GetCharWidth(int theChar)
{
	if(theChar<32 || theChar>=255)
		theChar = 127;

	return mCharWidth[theChar];
}

