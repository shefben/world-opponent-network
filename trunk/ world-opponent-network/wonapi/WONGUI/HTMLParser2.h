#ifndef __WON_HTMLParser2_H__
#define __WON_HTMLParser2_H__

#include "TextBox.h"
#include "WONCommon/StringUtil.h"
#include <stdio.h>
#include <string>
#include <map>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTMLParser2
{
protected:
	enum TagType
	{
		TagType_None,
		TagType_Anchor,
		TagType_LineBreak,
		TagType_Paragraph,
		TagType_Bold,
		TagType_Italic,
		TagType_Underline,
		TagType_H1,
		TagType_H2,
		TagType_H3,
		TagType_H4,
		TagType_H5,
		TagType_H6,
		TagType_Code,
		TagType_ListItem,
		TagType_Body,
		TagType_Image,
		TagType_Pre,
		TagType_Text,
	};

protected:
	TextAreaPtr mDisplay;
	FILE *mFile;
	std::string mBackStr;
	int mLineNum;
	std::string mPath;

	std::string mCurTagName;
	TagType mCurTagType;
	bool mIsEndTag;
	std::string mCurText;
	std::string mCurLinkParam;
	bool mHadLineBreak;
	bool mHadTextSpace;
	typedef std::map<std::string,TagType,StringLessNoCase> TagNameMap;
	TagNameMap mTagNameMap;

	typedef std::map<std::string,std::string,StringLessNoCase> TagAttributes;
	TagAttributes mCurTagAttributes;

	int mHeaderCount;
	int mBoldCount;
	int mItalicCount;
	int mUnderlineCount;
	bool mInAnchor;
	bool mInBody;
	bool mInPre;
	typedef std::vector<FontPtr> FontStack;
	FontStack mFontStack;
	FontPtr mCurFont;

	void CloseFile();
	bool EndOfFile();
	int GetChar();
	int PeekChar();
	void UngetChar(int theChar);	
	void UngetString(const std::string &theStr);
	bool SkipWhitespace();
	int ReadEntity();
	bool ReadWord(std::string &theWord, const char *theStopChars = NULL, bool stopAtSpace = true, bool skip = false, bool erase = true);

	bool GetTextTag();
	bool GetActualTag();
	bool GetNextTag();

	std::string* GetParam(const char *theName);
	void CheckAlignment();
	bool HandleImage();
	void DoLineBreak();
	void CalcFont();
	void HandleStartTag();
	void HandleEndTag();

public:
	HTMLParser2(TextArea *theDisplay = NULL);

	bool Parse(const char *theFilePath, TextArea *theDisplay = NULL);

	void SetDisplay(TextArea *theDisplay) { mDisplay = theDisplay; }
	const std::string& GetPath() { return mPath; }
};


}; // namespace WONAPI

#endif