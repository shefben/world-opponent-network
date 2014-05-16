#ifndef __WON_FONT_H__
#define __WON_FONT_H__

#include <string>
#include <map>
#include "WONCommon/SmartPtr.h"
#include "GUIString.h"


namespace WONAPI
{
class DisplayContext;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum FontStyles
{
	FontStyle_Plain		= 0,
	FontStyle_Bold		= 1,
	FontStyle_Italic	= 2,
	FontStyle_Underline = 4
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct FontDescriptor
{
	std::string mFace;
	int mStyle;
	int mSize;

	FontDescriptor() : mStyle(0), mSize(0) { }
	FontDescriptor(const std::string &theFace, int theStyle, int theSize) : 
		mFace(theFace), mStyle(theStyle), mSize(theSize) {}

	bool operator<(const FontDescriptor &d) const;
	bool operator==(const FontDescriptor &d) const;
	bool operator!=(const FontDescriptor &d) const;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Font : public WONAPI::RefCount
{
protected:
	FontDescriptor mDescriptor;
	void DrawUnderline(DisplayContext *theContext, const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y);

public:
	Font(DisplayContext *theContext, const FontDescriptor &theDescriptor);
	virtual ~Font();

	virtual void PrepareContext(DisplayContext * /*theContext*/) { }
	virtual void DrawString(DisplayContext * /*theContext*/, const GUIString &/*theStr*/, unsigned short /*theOffset*/, unsigned short /*theLength*/, int /*x*/, int /*y*/) { }

public:
	const FontDescriptor& GetDescriptor() { return mDescriptor; }

	int GetSize() { return mDescriptor.mSize; }
	int GetStyle() { return mDescriptor.mStyle; }
	const std::string& GetFace() { return mDescriptor.mFace; }

	int GetStringWidth(const GUIString &theStr) { return GetStringWidth(theStr,0,(unsigned short)theStr.length()); }
	int GetStringWidth(const char *theStr, int theLen) { return GetStringWidth(GUIString(theStr,(unsigned short)theLen)); }
	virtual int GetStringWidth(const GUIString &/*theStr*/, unsigned short /*theOffset*/, unsigned short /*theLength*/) { return 0; }
	virtual int GetCharWidth(int /*theChar*/) { return 0; }

	virtual int GetHeight() { return 0; }
	virtual int GetAscent() { return 0; }
	virtual int GetDescent() { return 0; }
};

typedef WONAPI::SmartPtr<Font> FontPtr;
typedef std::map<FontDescriptor, FontPtr> FontMap;

}; // namespace WONAPI

#endif