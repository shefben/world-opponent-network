#ifndef __WON_FTFONT_H__
#define __WON_FTFONT_H__

#include "WONGUI/Font.h"

struct FT_FaceRec_;
typedef struct FT_FaceRec_*  FT_Face;
typedef struct FT_BitmapGlyphRec_*  FT_BitmapGlyph;

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FTFont : public Font
{
protected:
	unsigned short mHeight;
	unsigned short mAscent;
	unsigned short mDescent;

	enum { CACHE_SIZE = 128 };

	FT_Face mFace;
	FT_BitmapGlyph mGlyphCache[CACHE_SIZE+1];

	void DrawString(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y);

public:
	FTFont(const FontDescriptor &theDescriptor);

	virtual int GetStringWidth(const GUIString &theStr, unsigned short theOffset, unsigned short theLength);
	virtual int GetCharWidth(int theChar);

	virtual int GetHeight() { return mHeight; }
	virtual int GetAscent() { return mAscent; }
	virtual int GetDescent() { return mDescent; }

	FT_Face GetFTFace() { return mFace; }
	FT_BitmapGlyph GetGlyph(int theChar);

	static void SetFontDir(const std::string &theDir);
};
typedef SmartPtr<FTFont> FTFontPtr;

} // namespace WONAPI

#endif