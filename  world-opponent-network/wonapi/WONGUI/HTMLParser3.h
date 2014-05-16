#ifndef __WON_HTMLPARSER_H__
#define __WON_HTMLPARSER_H__

#include "Table.h"
#include "WONCommon/StringUtil.h"
#include "NativeImage.h"
#include "Animation.h"
#include "MSControls.h"
#include <stdio.h>
#include <string>
#include <map>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTMLParser
{
protected:
	enum TagType
	{
		TagType_None,
		TagType_Anchor,
		TagType_LineBreak,
		TagType_HorizontalRule,
		TagType_Paragraph,
		TagType_Div,
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
		TagType_OrderedList,
		TagType_UnorderedList,
		TagType_ListItem,
		TagType_Body,
		TagType_Image,
		TagType_Pre,
		TagType_Text,
		TagType_Font,
		TagType_BaseFont,
		TagType_Center,
		TagType_Table,
		TagType_TableRow,
		TagType_TableData,
		TagType_TableHeader,
		TagType_Small,
		TagType_Big,
		TagType_NoBreak,
		TagType_Input,
		TagType_Select,
		TagType_Option,
		TagType_ImageMap,
		TagType_Area,
		TagType_Script,
		TagType_Style,
		TagType_Max
	};

protected:
	MultiLineElementPtr mDisplay;
	TablePtr mDisplayContainer;
	WindowPtr mWindow;
	FILE *mFile;
	const char *mText;
	GUIString mBackStr;
	int mLineNum;
	std::string mPath;
	int mInTagCount[TagType_Max];

	typedef std::string(*DocumentFetcher)(const std::string &thePath, const std::string &theBasePath);
	static std::string DefaultDocumentFetcher(const std::string &thePath, const std::string &theBasePath);
	DocumentFetcher mDocumentFetcher;

	typedef ImagePtr(*ImageFetcher)(const std::string &thePath, const std::string &theBasePath);
	ImageFetcher mImageFetcher;

	GUIString mCurTagName;
	TagType mCurTagType;
	bool mIsEndTag;
	GUIString mCurText;
	TextLinkParamPtr mCurLinkParam;

	LineSegmentPtr mPrevSegment;
	LineBuilder mCurLine;
	GUIString mLastWord;
	bool mHadSpaceOnEnd;
	void ClearAccumulatedSegment();
	void AddAccumulatedSegment();
	TextLineSegment* GetTextLineSegment(const GUIString &theStr, int theColor, Font *theFont);

	bool mHadLineBreak, mHadParagraphBreak;
	bool mInParagraph;
	typedef std::map<GUIString,TagType,GUIStringLessNoCase> TagNameMap;
	static TagNameMap& GetTagNameMap();
	TagNameMap &mTagNameMap;

	typedef std::map<GUIString,GUIString,StringLessNoCase> TagAttributes;
	TagAttributes mCurTagAttributes;

	int mBoldCount;
	int mItalicCount;
	int mUnderlineCount;
	int mLineType;
	bool mInAnchor;
	bool mInBody;
	bool mInPre;
	bool mInNoBreak;
	bool mParsingTableData;
	bool mInTable;
	bool mDone;
	bool mRichTextParse;
	TableElementPtr mCurTable;
	int mCurTableAlign;

	MSComboBoxPtr mCurCombo;
	ImageMapPtr mCurImageMap;

	int mLeftMargin, mRightMargin, mTopMargin, mBottomMargin;

	struct ListInfo
	{
		bool mOrdered;
		int mType;
		int mCount;
	};
	typedef std::vector<ListInfo> ListStack;
	ListStack mListStack;
	void HandleOrderedList();
	void HandleUnorderedList();
	void HandleEndList();
	void HandleListItem();

	void HandleTable();
	void HandleTableRow();
	void HandleTableData(bool isHeader);
	void HandleEndTableData();

	void HandleInput();
	void HandleSelect();
	void HandleOption();

	void HandleHorizontalRule();
	void HandleArea();

	struct FontInfo
	{
		std::string mFace;
		int mSize;
		int mColor;

		FontInfo() { }
		FontInfo(const std::string &theFace, int theSize, int theColor) : mFace(theFace), mSize(theSize), mColor(theColor) { }
	};
	typedef std::vector<FontInfo> FontStack;
	FontStack mFontStack;
	FontPtr mCurFont;
	int mCurColor;
	int mDefaultLinkColor, mDefaultALinkColor, mDefaultTextColor;
	std::string mDefaultFontFace;
	HorzAlign mDefaultAlign;

	typedef std::vector<HorzAlign> AlignStack;
	AlignStack mAlignStack;
	HorzAlign GetCurAlign();
	void PushAlign(HorzAlign theAlign, bool force=false);
	void PopAlign();
	void CheckAlignment();

	void CloseFile();
	inline bool EndOfFile();
	inline int GetChar();
	inline int PeekChar();
	inline void UngetChar(int theChar);	
	inline void UngetString(const GUIString &theStr);
	inline bool SkipWhitespace();
	int ReadEntity();
	bool ReadWord(GUIString &theWord, const char *theStopChars = NULL, bool stopAtSpace = true, bool skip = false, bool erase = true);

	bool GetTextTag();
	bool GetActualTag();
	bool GetNextTag();

	bool GetIntParam(const GUIString &theName, int &theInt);
	int GetColor(GUIString *theStr);
	GUIString* GetParam(const GUIString &theName);
	int GetPercentValue(GUIString *theVal);
	NativeImagePtr GetImage(GUIString *theImagePath, AnimationPtr *theAnimation = NULL, RawImagePtr *theRawImage = NULL);
	int GetTableDataHAlign(GUIString *theAlign, int theDefault = 0);
	int GetTableDataVAlign(GUIString *theAlign);
	void HandleBody();
	bool HandleImage();
	void HandleFont(bool isBase);
	void PushFont(const char *theFace, int theSize, int theColor);
	void PopFont();
	FontInfo& GetFontInfo();
	int GetFontStyle();
	void CalcFont();
	void HandleStartTag();
	void HandleEndTag();

	void AddParagraphBreak(bool unconditional = false, int extraFlags = 0);
	void AddLineBreak(int extraFlags = 0);
	void AddDisplaySegment(LineSegment *theSegment, int theFlags = 0, bool clearAccum = true);
	void CheckNewContent();

protected:
	bool Parse();
	void CopyFileInfo(const HTMLParser &from);
	void CopyTagInfo(const HTMLParser &from);
	bool ParseTableData(HTMLParser &theParser, MultiLineElement *theDisplay, bool isHeader, HorzAlign theStartAlign);

	bool UserParse(MultiLineElement *theDisplay);

public:
	HTMLParser(MultiLineElement *theDisplay = NULL);
	void SetDocumentFetcher(DocumentFetcher theFetcher);
	void SetImageFetcher(ImageFetcher theFetcher) { mImageFetcher = theFetcher; }

	bool Parse(const char *theFilePath, MultiLineElement *theDisplay = NULL);
	bool ParseText(const char *theText, MultiLineElement *theDisplay = NULL);
	bool ParseRichText(const char *theText, int theDefaultColor, Font *theDefaultFont, MultiLineElement *theDisplay = NULL);

	void SetDisplay(MultiLineElement *theDisplay, Table *theContainer) { mDisplay = theDisplay; mDisplayContainer = theContainer; }
	const std::string& GetPath() { return mPath; }
};


}; // namespace WONAPI

#endif