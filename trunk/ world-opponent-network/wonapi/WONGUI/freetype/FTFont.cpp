#include "FTFont.h"
#include "WONGUI/SDisplayContext.h"

#include "WONCommon/ReadBuffer.h"

#include "ft2build.h"
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>

using namespace WONAPI;

static FT_Library gLibrary = NULL;
static std::string gFontDir = "\\\\noodle\\c\\winnt\\fonts\\";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FTFont::SetFontDir(const std::string &theDir)
{
	gFontDir = theDir;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static std::string GetFontFilePath(const FontDescriptor &theDescriptor)
{
	GUIString aStr = theDescriptor.mFace;
	int aStyle = theDescriptor.mStyle;
	bool bold = aStyle&FontStyle_Bold?true:false;
	bool italic = aStyle&FontStyle_Italic?true:false;

	std::string aPath = gFontDir;
	if(aStr.findNoCase("Times New Roman")>=0)
	{
		if(bold && italic)
			return aPath + "timesbi.ttf";
		else if(bold)
			return aPath + "timesbd.ttf";
		else if(italic)
			return aPath + "timesi.ttf";
		else
			return aPath + "times.ttf";
	}
	else if(aStr.findNoCase("Tahoma")>=0)
	{
		if(bold)
			return aPath + "tahomabd.ttf";
		else
			return aPath + "tahoma.ttf";
	}
	else if(aStr.findNoCase("Courier")>=0)
	{
		if(bold && italic)
			return aPath + "courbi.ttf";
		else if(bold)
			return aPath + "courbd.ttf";
		else if(italic)
			return aPath + "couri.ttf";
		else
			return aPath + "cour.ttf";
	}
	else if(aStr.findNoCase("verdana")>=0)
	{
		if(bold && italic)
			return aPath + "verdanaz.ttf";
		else if(bold)
			return aPath + "verdanab.ttf";
		else if(italic)
			return aPath + "verdanai.ttf";
		else
			return aPath + "verdana.ttf";
	}

	if(bold && italic)
		return aPath + "arialbi.ttf";
	else if(bold)
		return aPath + "arialbd.ttf";
	else if(italic)
		return aPath + "ariali.ttf";
	else
		return aPath + "arial.ttf";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FTFont::FTFont(const FontDescriptor &theDescriptor) : Font(NULL, theDescriptor)
{
	FT_Error error = 0;
	if(gLibrary==NULL)
		error = FT_Init_FreeType(&gLibrary);

    error = FT_Init_FreeType(&gLibrary);

	std::string aPath = GetFontFilePath(theDescriptor);
    error = FT_New_Face( gLibrary,
                         aPath.c_str(),
                         0,
                         &mFace);      

	int aHeight = MulDiv(theDescriptor.mSize, 96, 72);
	int aWidth = aHeight;
    error = FT_Set_Pixel_Sizes(
              mFace,    /* handle to face object           */
              aWidth,       /* char_width in 1/64th of points  */
              aHeight /* char_height in 1/64th of points */
	);


	mAscent = mFace->size->metrics.ascender/64;
	mDescent = -mFace->size->metrics.descender/64;
	mHeight = mAscent+mDescent; //mFace->size->metrics.height/64;

	for(int i=0; i<CACHE_SIZE+1; i++)
		mGlyphCache[i] = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
static void Surface_DrawString_Mono(Surface<PixelType> *theSurface, FTFont *theFont, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	int mClipX1, mClipX2, mClipY1, mClipY2;
	theSurface->GetClipRect(mClipX1,mClipY1,mClipX2,mClipY2);
	if(x>mClipX2 || y>mClipY2 || y+theFont->GetHeight()<=mClipY1)
		return;

	ReadBuffer aBuf;
	

//	int aHeight = theFont->GetHeight();
	int mPixelsPerRow = theSurface->GetPixelsPerRow();
	PixelType *anUpperLeftCorner = theSurface->GetPixels() + y*mPixelsPerRow + x;
	PixelType *aRow, *aPixel;
	FT_Face mFace = theFont->GetFTFace();
	for(int i=0; i<theLength; i++)
	{
		int aChar = theStr.at(theOffset+i);
		int aWidth,aHeight;
		if(x>=mClipX1)
		{
//			FT_Load_Char(mFace, aChar, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
			FT_BitmapGlyph aGlyph = theFont->GetGlyph(aChar);

			FT_Bitmap *aBitmap = &aGlyph->bitmap;
			const unsigned char *aBuffer = aBitmap->buffer;

			aHeight = aBitmap->rows;
			aWidth = aBitmap->width;
			int aNumPadBytes = abs(aBitmap->pitch) - aWidth/8 - 1;

//			int tx = mFace->glyph->bitmap_left;
//			int ty = theFont->GetAscent()-mFace->glyph->bitmap_top;
			int tx = aGlyph->left;
			int ty = theFont->GetAscent()-aGlyph->top;
			aRow = anUpperLeftCorner + tx + ty*mPixelsPerRow; 
			for(int i=0; i<aHeight; i++)
			{
				int curMask = 128;
				unsigned char curByte = *aBuffer++;

				aPixel = aRow;
				for(int j=0; j<aWidth; j++)
				{
					if(curByte&curMask)
						theSurface->PutPixel(x+tx+j,y+ty+i,aPixel);

					curMask>>=1;
					if(curMask==0)
					{
						curByte = *aBuffer++;
						curMask = 128;
					}
						
					aPixel++;
				}
				aRow+=mPixelsPerRow;
				aBuffer += aNumPadBytes;
			}

//			aWidth = (mFace->glyph->advance.x /*+ mFace->glyph->metrics.horiAdvance*/)>>6;
			aWidth = (aGlyph->root.advance.x /*+ mFace->glyph->metrics.horiAdvance*/)>>6;
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
#define COLOR_1_8(c) (((c)>>3)&0xff1f1f1f)
#define COLOR_2_8(c) (((c)>>2)&0xff3f3f3f)
#define COLOR_3_8(c) (COLOR_1_8(c)+COLOR_2_8(c))
#define COLOR_4_8(c) (((c)>>1)&0xff7f7f7f)
#define COLOR_5_8(c) (COLOR_4_8(c)+COLOR_1_8(c))
#define COLOR_6_8(c) (COLOR_4_8(c)+COLOR_2_8(c))
#define COLOR_7_8(c) (COLOR_4_8(c)+COLOR_3_8(c))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline int HighestPowerOf2(unsigned char blend)
{
	int r = 0;
	if(blend&0xf0) { r+=4; blend>>=4; }
	if(blend&0x0c) { r+=2; blend>>=2; }
	if(blend&0x02) { r++; }

	return r;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline DWORD BlendColors32Fast(DWORD c1, DWORD c2, unsigned char blend)
{
	switch((blend + 16)>>5)
	{
		case 0: return c2;
		case 1: return COLOR_1_8(c1) + COLOR_7_8(c2);
		case 2: return COLOR_2_8(c1) + COLOR_6_8(c2);
		case 3: return COLOR_3_8(c1) + COLOR_5_8(c2);
		case 4: return COLOR_4_8(c1) + COLOR_4_8(c2);
		case 5: return COLOR_5_8(c1) + COLOR_3_8(c2);
		case 6: return COLOR_6_8(c1) + COLOR_2_8(c2);
		case 7: return COLOR_7_8(c1) + COLOR_1_8(c2);
		case 8: return c1;
	}

	return c1;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline DWORD BlendColors32(DWORD c1, DWORD c2, unsigned char blend)
{
	if(blend==255)
		return c1;
	else if(blend==0)
		return c2;


	int r1 = (c1&0xff0000)>>16;
	int g1 = (c1&0xff00)>>8;
	int b1 = (c1&0xff);

	int r2 = (c2&0xff0000)>>16;
	int g2 = (c2&0xff00)>>8;
	int b2 = (c2&0xff);

	int r3 = ((blend*r1 + (255-blend)*r2)/255)&0xff;
	int g3 = ((blend*g1 + (255-blend)*g2)/255)&0xff;
	int b3 = ((blend*b1 + (255-blend)*b2)/255)&0xff;

	return (r3<<16) | (g3<<8) | b3;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline unsigned short BlendColors565(unsigned short oc1, unsigned short oc2, unsigned char blend)
{
	if(blend==255)
		return oc1;
	else if(blend==0)
		return oc2;

	DWORD c1 = SURFACE_565_TO_32(oc1);
	DWORD c2 = SURFACE_565_TO_32(oc2);

	int r1 = (c1&0xff0000)>>16;
	int g1 = (c1&0xff00)>>8;
	int b1 = (c1&0xff);

	int r2 = (c2&0xff0000)>>16;
	int g2 = (c2&0xff00)>>8;
	int b2 = (c2&0xff);

	int r3 = ((blend*r1 + (255-blend)*r2)/255)&0xff;
	int g3 = ((blend*g1 + (255-blend)*g2)/255)&0xff;
	int b3 = ((blend*b1 + (255-blend)*b2)/255)&0xff;

	c1 = (r3<<16) | (g3<<8) | b3;
	return SURFACE_32_TO_565(c1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PixelType>
static void Surface_DrawString_AA(Surface<PixelType> *theSurface, FTFont *theFont, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	int mClipX1, mClipX2, mClipY1, mClipY2;
	theSurface->GetClipRect(mClipX1,mClipY1,mClipX2,mClipY2);
	if(x>mClipX2 || y>mClipY2 || y+theFont->GetHeight()<=mClipY1)
		return;

	int mPixelsPerRow = theSurface->GetPixelsPerRow();
	PixelType *anUpperLeftCorner = theSurface->GetPixels() + y*mPixelsPerRow + x;
	PixelType *aRow, *aPixel;
	FT_Face mFace = theFont->GetFTFace();
	PixelType aColor = theSurface->GetColor();
	for(int i=0; i<theLength; i++)
	{
		int aChar = theStr.at(theOffset+i);
		int aWidth,aHeight;
		if(x>=mClipX1)
		{
			FT_BitmapGlyph aGlyph = theFont->GetGlyph(aChar);

			FT_Bitmap *aBitmap = &aGlyph->bitmap;
			const unsigned char *aBuffer = aBitmap->buffer;

			aHeight = aBitmap->rows;
			aWidth = aBitmap->width;
			int aNumPadBytes = abs(aBitmap->pitch) - aWidth;

			int tx = aGlyph->left;
			int ty = theFont->GetAscent()-aGlyph->top;
			aRow = anUpperLeftCorner + tx + ty*mPixelsPerRow; 
			for(int i=0; i<aHeight; i++)
			{
				int curMask = 128;

				aPixel = aRow;
				for(int j=0; j<aWidth; j++)
				{
					unsigned char curByte = *aBuffer++;						
					if(theSurface->NotClipped(x+tx+j,y+ty+i))
					{
//						*aPixel = BlendColors32Fast(aColor,*aPixel,curByte); 
						switch(theSurface->GetSurfaceFormat())
						{
							case SurfaceFormat_32: *aPixel = BlendColors32(aColor,*aPixel,curByte); break;
							case SurfaceFormat_565: *aPixel = BlendColors565(aColor,*aPixel,curByte); break;
						}
					}

					aPixel++;
				}
				aRow+=mPixelsPerRow;
				aBuffer += aNumPadBytes;
			}

			aWidth = aGlyph->root.advance.x>>6;
		}
		else
			aWidth=theFont->GetCharWidth(aChar);

		x+=aWidth;
		anUpperLeftCorner+=aWidth;

		if(x>mClipX2)
			return;
	}
}

//#define ANTIALIAS
#ifdef ANTIALIAS
#define DRAWSTR Surface_DrawString_AA
#define RENDERFLAGS FT_LOAD_RENDER
#else
#define DRAWSTR Surface_DrawString_Mono
#define RENDERFLAGS FT_LOAD_RENDER | FT_LOAD_MONOCHROME
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FTFont::DrawString(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	SurfaceBase *aSurface = ((SDisplayContext*)theContext)->mSurface;
	switch(aSurface->GetSurfaceFormat())
	{
		case SurfaceFormat_555:
		case SurfaceFormat_565:
			DRAWSTR<unsigned short>((Surface16*)aSurface,this,theStr,theOffset,theLength,x,y);
			break;
		
		case SurfaceFormat_32:
			DRAWSTR<DWORD>((Surface32*)aSurface,this,theStr,theOffset,theLength,x,y);
			break;
	}

//	aSurface->DrawString(this,theStr,theOffset,theLength,x,y);

	if(GetStyle() & FontStyle_Underline)
		DrawUnderline(theContext,theStr,theOffset,theLength,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FTFont::GetStringWidth(const GUIString &theStr, unsigned short theOffset, unsigned short theLength)
{
	int aWidth = 0;
	const int anEnd = theLength+theOffset;
	for(int i=theOffset; i<anEnd; i++)
		aWidth += GetCharWidth(theStr.at(i));

	return aWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FTFont::GetCharWidth(int theChar)
{
//    FT_Error anError = FT_Load_Char( mFace, theChar, FT_LOAD_DEFAULT);
//	return (mFace->glyph->advance.x  /*+ mFace->glyph->metrics.horiAdvance*/) >>6;

	FT_BitmapGlyph aGlyph = GetGlyph(theChar);
	return aGlyph->root.advance.x >> 6;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FT_BitmapGlyph FTFont::GetGlyph(int theChar)
{
	if(theChar>=0 && theChar<CACHE_SIZE)
	{
		if(mGlyphCache[theChar]==NULL)
		{
		    FT_Error anError = FT_Load_Char( mFace, theChar, RENDERFLAGS);
			FT_Glyph aGlyph;
			FT_Get_Glyph(mFace->glyph, &aGlyph);
			aGlyph->advance = mFace->glyph->advance;
			mGlyphCache[theChar] = (FT_BitmapGlyph)aGlyph;
		}
		return mGlyphCache[theChar];
	}


	FT_Glyph aGlyph = (FT_Glyph)mGlyphCache[CACHE_SIZE];
	if(aGlyph!=NULL)
	{
		FT_Done_Glyph(aGlyph);
		mGlyphCache[CACHE_SIZE] = NULL;
	}

	FT_Error anError = FT_Load_Char( mFace, theChar, RENDERFLAGS);
	FT_Get_Glyph(mFace->glyph, &aGlyph);
	aGlyph->advance = mFace->glyph->advance;
	mGlyphCache[CACHE_SIZE] = (FT_BitmapGlyph)aGlyph;

	return (FT_BitmapGlyph)aGlyph;
}

