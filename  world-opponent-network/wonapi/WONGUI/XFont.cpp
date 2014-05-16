#include "DisplayContext.h"
#include "XFont.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static unsigned long RankFont(const WONAPI::FontDescriptor &theDescriptor, const char *theName)
{
	const char *weights[] = { "light", "medium", "demibold", "bold", "black"};

	int weight = 0;
	int i;
	char *aPtr = NULL;
	for(i=0; i<5; i++)
	{
		aPtr = strstr(theName, weights[i]);
		if(aPtr!=NULL)
		{	
			weight = i;
			aPtr += strlen(weights[i]);
			break;
		}
	}	

	if(aPtr==NULL || aPtr[0]=='\0')
		return 0xffffffff;

	bool italic = aPtr[1]=='o' || aPtr[1]=='i';

	aPtr = strstr(aPtr,"--");
	if(aPtr==NULL)
		return 0xffffffff;

	aPtr+=2;
	int aSize = 0;
	if(sscanf(aPtr, "%d", &aSize)!=1)
		return 0xffffffff;

	int score = 0;
	int dsize = theDescriptor.mSize*4/3 - aSize;
	if(dsize < 0)
		score = -dsize*2;
	else 
		score = dsize;

	// Weight font size more highly than bold/italic
	score <<= 16;

	bool wantItalic = (theDescriptor.mStyle&WONAPI::FontStyle_Italic)?true:false;
	bool wantBold = (theDescriptor.mStyle&WONAPI::FontStyle_Bold)?true:false;
	if(italic!=wantItalic)
		score += 1;

	if(wantBold)
	{
		switch(weight)
		{
			case 0: score += 8;
			case 1: score += 7;
			case 2: score += 6;
			case 3: score += 1;
		}
	}
	else
	{
		switch(weight)
		{
			case 0: score += 2;
			case 2: score += 1;
			case 3: score += 3;
			case 4: score += 4;	
		}
	}
 				
	return score;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::XFont::XFont(DisplayContext *theContext, const FontDescriptor &theDescriptor) : Font(theContext, theDescriptor)
{
	Display *theDisplay = theContext->mDisplay;

	std::string aName = "*" + theDescriptor.mFace + "*";
	int aCount = 0;
	char **fonts = XListFonts(theDisplay,aName.c_str(),1000,&aCount);
	if(fonts==NULL)
		fonts = XListFonts(theDisplay,"*helvetica*",1000,&aCount);

	int best = 0;
	unsigned long bestScore = 0xffffffff;
	for(int i=0; i<aCount; i++)
	{
		unsigned long aScore = RankFont(theDescriptor, fonts[i]);
		if(aScore < bestScore)
		{
			best = i;
			bestScore = aScore;
		}	
	}		

	aName = fonts[best];
	XFreeFontNames(fonts); 

	mDescriptor = theDescriptor;
	mFont = XLoadFont(theDisplay, aName.c_str());
	mFontStruct = XQueryFont(theDisplay, mFont);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::XFont::~XFont()
{
	XFreeFontInfo(NULL, mFontStruct, 0);
//	XUnloadFont(theDisplay, mFont);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XFont::PrepareContext(DisplayContext *theContext) 
{
	XSetFont(theContext->mDisplay, theContext->mGC, mFont);
}
	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XFont::DrawString(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	XDrawString(theContext->mDisplay, theContext->mDrawable, theContext->mGC, x, y + GetAscent(), ((const char*)theStr.GetInternalData())+theOffset, theLength);
	if(GetStyle() & FontStyle_Underline)
	{
		Graphics *g = theContext->mGraphics;
		if(g!=NULL)
			g->DrawLineHook(x, y+GetAscent()+1, x+GetStringWidth(theStr, theOffset,theLength), y+GetAscent()+1);
//		DrawLine(oldx, oldy + mFont->GetAscent(), oldx + mFont->GetStringWidth(theStr), oldy + mFont->GetAscent());	
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONAPI::XFont::GetStringWidth(const GUIString &theStr, unsigned short theOffset, unsigned short theLength)
{
	if(theLength==0)
		return 0;

	return XTextWidth(mFontStruct, ((const char*)theStr.GetInternalData())+theOffset, theLength);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONAPI::XFont::GetCharWidth(int theChar)
{
	char aChar = theChar;
	return XTextWidth(mFontStruct, &aChar, 1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONAPI::XFont::GetHeight()
{
	return mFontStruct->ascent + mFontStruct->descent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONAPI::XFont::GetAscent()
{
	return mFontStruct->ascent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONAPI::XFont::GetDescent()
{
	return mFontStruct->descent;
}
