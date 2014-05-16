#ifndef __WON_MSFONT_H__
#define __WON_MSFONT_H__

#include "Font.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSFont : public Font
{
protected:
	HFONT mFont;
	HDC mDC;
	TEXTMETRIC mTextMetrics;

public:
	MSFont(DisplayContext *theContext, const FontDescriptor &theDescriptor);
	virtual ~MSFont();

	virtual void PrepareContext(DisplayContext *theContext);
	virtual void DrawString(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y);

public:
	virtual int GetStringWidth(const GUIString &theStr, unsigned short theOffset, unsigned short theLength);
	virtual int GetCharWidth(int theChar);

	virtual int GetHeight();
	virtual int GetAscent();
	virtual int GetDescent();
};
typedef WONAPI::SmartPtr<MSFont> MSFontPtr;

};	// namespace WONAPI

#endif