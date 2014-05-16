#include "MSFont.h"
#include "MSDisplayContext.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSFont::MSFont(DisplayContext *theContext, const FontDescriptor &theDescriptor) : 
	Font(theContext, theDescriptor)
{
	mDC = CreateCompatibleDC(((MSDisplayContext*)theContext)->mDC);
//	int nHeight = -MulDiv(theDescriptor.mSize, GetDeviceCaps(mDC, LOGPIXELSY), 72);
	int nHeight = -MulDiv(theDescriptor.mSize, 96, 72); // GetDeviceCaps(mDC, LOGPIXELSY)==96 for Small Fonts System

	int aStyle = theDescriptor.mStyle;
	mFont = CreateFont(
		nHeight,											// height
		0,													// width
		0,													// escapement
		0,													// orientation
		(aStyle&FontStyle_Bold)?FW_BOLD:FW_NORMAL,			// weight
		(aStyle&FontStyle_Italic)?TRUE:FALSE,				// italic
		FALSE,/* (aStyle&FontStyle_Underline)?TRUE:FALSE,/**/	// underline
		FALSE,												// strikeout
		ANSI_CHARSET,										// character set
		OUT_DEFAULT_PRECIS,									// output precision
		CLIP_DEFAULT_PRECIS,								// clipping precision
		DEFAULT_QUALITY,									// quality
		FF_DONTCARE,										// family
		theDescriptor.mFace.c_str());

	SelectObject(mDC, mFont);
	GetTextMetrics(mDC, &mTextMetrics);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSFont::~MSFont()
{
	DeleteObject(mFont);
	DeleteDC(mDC);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFont::PrepareContext(DisplayContext *theContext)
{
	SelectObject(((MSDisplayContext*)theContext)->mDC, mFont);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFont::DrawString(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
#ifdef GUISTRING_ASCII
	TextOutA(((MSDisplayContext*)theContext)->mDC,x,y,((char*)theStr.GetInternalData())+theOffset,theLength);
#else
	TextOutW(((MSDisplayContext*)theContext)->mDC,x,y,((wchar_t*)theStr.GetInternalData())+theOffset,theLength);
#endif

	if(GetStyle() & FontStyle_Underline)
		DrawUnderline(theContext,theStr,theOffset,theLength,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MSFont::GetStringWidth(const GUIString &theStr, unsigned short theOffset, unsigned short theLen)
{
	if(theLen==0)
		return 0;

	SIZE aSize;

#ifdef GUISTRING_ASCII
	GetTextExtentPoint32A(mDC, ((char*)theStr.GetInternalData())+theOffset, theLen, &aSize);
#else
	GetTextExtentPoint32W(mDC, ((wchar_t*)theStr.GetInternalData())+theOffset, theLen, &aSize);
#endif

	return aSize.cx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MSFont::GetCharWidth(int theChar)
{

	int aWidth = 0;
	::GetCharWidthW(mDC, theChar, theChar, &aWidth);
	return aWidth;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MSFont::GetHeight()
{
	return mTextMetrics.tmHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MSFont::GetAscent()
{
	return mTextMetrics.tmAscent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MSFont::GetDescent()
{
	return mTextMetrics.tmDescent;
}


