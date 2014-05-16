#ifndef __WON_XFONT_H__
#define __WON_XFONT_H__

#include "GUISystem.h"
#include "Font.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XFont : public Font
{
protected:
	::Font mFont;
	XFontStruct *mFontStruct;

	virtual ~XFont();

public:
	virtual void PrepareContext(DisplayContext *theContext);
	virtual void DrawString(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y);

public:
	XFont(DisplayContext *theContext, const FontDescriptor &theDescriptor);

	virtual int GetStringWidth(const GUIString &theStr, unsigned short theOffset, unsigned short theLength);
	virtual int GetCharWidth(int theChar);

	virtual int GetHeight();
	virtual int GetAscent();
	virtual int GetDescent();
};
typedef WONAPI::SmartPtr<XFont> XFontPtr;

};

#endif
