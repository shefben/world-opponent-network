#ifndef __WON_SFONT_H__
#define __WON_SFONT_H__

#include "WONCommon/ByteBuffer.h"
#include "WONGUI/Font.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SFont : public Font
{
protected:
	unsigned short mCharWidth[255];
	char *mCharData[255];
	ByteBufferPtr mFontData;
	unsigned short mHeight;
	unsigned short mAscent;
	unsigned short mDescent;

	void DrawString(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y);

public:
	SFont(const FontDescriptor &theDescriptor);
	bool Read(const char *theFilePath);

	void* GetCharData(int theChar);
	void* GetCharData(int theChar, int *theWidth);

	virtual int GetStringWidth(const GUIString &theStr, unsigned short theOffset, unsigned short theLength);
	virtual int GetCharWidth(int theChar);

	virtual int GetHeight() { return mHeight; }
	virtual int GetAscent() { return mAscent; }
	virtual int GetDescent() { return mDescent; }

};
typedef SmartPtr<SFont> SFontPtr;

} // namespace WONAPI

#endif