#include "Font.h"
#include "WONCommon/StringUtil.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::FontDescriptor::operator<(const FontDescriptor &d) const
{
	if(mSize < d.mSize)
		return true;

	if(mSize > d.mSize)
		return false;

	// mSize == d.mSize
	if(mStyle < d.mStyle)
		return true;

	if(mStyle > d.mStyle)
		return false;

	// mStyle == d.mStyle

	return WONAPI::StringCompareNoCase(mFace,d.mFace)<0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::FontDescriptor::operator==(const FontDescriptor &d) const
{
	if(mSize!=d.mSize)
		return false;

	if(mStyle!=d.mStyle)
		return false;

	return WONAPI::StringCompareNoCase(mFace,d.mFace)==0;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::FontDescriptor::operator!=(const FontDescriptor &d) const
{
	return !(*this==d);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::Font::Font(DisplayContext *theContext, const FontDescriptor &theDescriptor) : mDescriptor(theDescriptor)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::Font::~Font()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Font::DrawUnderline(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	WONAPI::Graphics *g = theContext->mGraphics;
	if(g!=NULL)
	{
		int aDescent = GetDescent();
		int aDescentMod;
		if(aDescent<3)
			aDescentMod = 0;
		else if(aDescent==3)
			aDescentMod = 1;
		else 
			aDescentMod = aDescent>>1;

		int aYPos = y+(GetHeight()-aDescentMod-1);
		g->DrawLineHook(x, aYPos, x+GetStringWidth(theStr,theOffset,theLength), aYPos);
	}
}

