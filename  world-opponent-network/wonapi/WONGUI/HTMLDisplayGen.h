#ifndef __WON_HTMLDISPLAYGEN_H__
#define __WON_HTMLDISPLAYGEN_H__

#include "HTMLDocument.h"
#include "Table.h"
#include "ComboBox.h"

#include <vector>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum HTMLFontSize
{
	HTMLFontSize_Smallest,
	HTMLFontSize_Small,
	HTMLFontSize_Medium,
	HTMLFontSize_Large,
	HTMLFontSize_Largest
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTMLDisplayGen
{
protected:
	///////////////////////////////////////////
	// Document
	HTMLDocumentPtr mDocument;
	const HTMLTagList *mTagList;
	HTMLTagList::const_iterator mTagListItr;
	HTMLTagList::const_iterator mTagListEnd;
	HTMLTextTag *mNextTextTag;


	///////////////////////////////////////////
	// Display
	TablePtr mDisplayContainer;
	MultiLineElementPtr mDisplay;
	Window *mWindow;
	LineBuilder mCurLine;
	LineSegmentPtr mPrevSegment;
	bool mCurLineEmpty;
	bool mPrevLineEmpty;
	bool mCanBreak;
	bool mLastWasSpace;
	bool mInPre;
	int mNoBreak;
	bool mInParagraph;
	ComboBoxPtr mCurCombo;
	bool mNoBackground;
	HTMLFontSize mFontSize;

	TableElementPtr mCurTable;
	HTMLAlign mCurTableAlign;
	bool mDone;
	bool mInTable;
	bool mParsingTableData;

	void CalcLineEmpty(int theFlags, bool addedSegmentHasContent);
	void StartFreshLine();
	void StartFreshParagraph();
	void AddLineBreak(int extraFlags = 0);

	void AddDisplaySegment(LineSegment *theSegment, int theFlags = 0);
	void AddEmptySegment(LineSegment *theSegment, int theFlags = 0);
	void AddNonDisplaySegment(LineSegment *theSegment, int theFlags = 0);
	void ClearAccumulatedSegment(int theFlags = 0);

	///////////////////////////////////////////
	// Links
	bool mInAnchor;
	TextLinkParamPtr mCurLinkParam;

	///////////////////////////////////////////
	// Fonts/TextColor
	struct FontInfo
	{
		std::string mFace;
		int mSize;
		int mColor;
		bool mIsMonoSpace;

		FontInfo() { }
		FontInfo(const std::string &theFace, int theSize, int theColor, bool isMonoSpace) : 
			mFace(theFace), mSize(theSize), mColor(theColor), mIsMonoSpace(isMonoSpace) { }
	};
	typedef std::vector<FontInfo> FontStack;
	FontStack mFontStack;
	FontPtr mCurFont;
	int mCurColor;
	int mBoldCount, mItalicCount, mUnderlineCount;
	int mDefaultLinkColor, mDefaultALinkColor, mDefaultTextColor;
	std::string mDefaultFontFace;

	void PushFont(const char *theFace, int theSize, int theColor, bool isMonoSpace = false);
	void PopFont();
	int GetFontStyle();
	bool GetIsFontMonospace();
	int GetFontSize();
	void CalcFont();

	///////////////////////////////////////////
	// Alignment
	HTMLAlign mDefaultAlign;
	typedef std::vector<HTMLAlign> AlignStack;
	AlignStack mAlignStack;
	HTMLAlign GetCurAlign();
	void PushAlign(HTMLAlign theAlign, bool force=false);
	void PopAlign();
	void CheckAlignment();


	///////////////////////////////////////////
	// Margin
	int mLeftMargin, mRightMargin, mTopMargin, mBottomMargin;
	int mListDepth;

	////////////////////////////////////
	HTMLTag* GetNextTag();
	void HandleBody(HTMLBodyTag *theTag);
	bool FindBody();

	///////////////////////////////////////////
	// Text context
	TextLineSegment* GetTextLineSegment(const GUIString &theStr, int theColor, Font *theFont);
	inline void HandlePreSpace(int theChar, GUIString &theText);
	int HandleBeginGlueWord(const GUIString &anOldText);
	HTMLTextTag* GetNextTextTag();
	
	void EndParagraph();
	void HandleEndTableData();

	void HandleGUIComponent(HTMLGUIComponentTag *theTag);
	void HandleOption(HTMLTag *theTag);
	void HandleInput(HTMLInputTag *theTag);
	void HandleTable(HTMLTableTag *theTag);
	void HandleTableRow(HTMLTableRowTag *theTag);
	void HandleTableData(HTMLTableDataTag *theTag);
	void HandleList(HTMLListTag* theTag);
	void HandleListItem(HTMLListItemTag* theTag);
	void HandleImage(HTMLImageTag* theTag);
//	void HandleArea(HTMLAreaTag* theTag);
//	void HandleImageMap(HTMLImageMapTag* theTag);
	void HandleBlock(HTMLBlockTag* theTag);
	void HandleFont(HTMLFontTag *theTag);
	void HandleModifyFontStyle(HTMLTag *theTag, int theStyleMod);
	void HandleModifyFont(HTMLTag *theTag, const char *theFace, int theSize, int theColor, bool isMonoSpace);
	void HandleAnchor(HTMLAnchorTag *theTag);
	void HandleHeading(HTMLHeadingTag *theTag);
	void HandleLineBreak(HTMLLineBreakTag *theTag);
	void HandleHorizontalRule(HTMLHorizontalRuleTag *theTag);
	void HandlePre(HTMLTag *theTag);
	void HandleCenter(HTMLTag *theTag);
	void HandleTextTag(HTMLTextTag *theTag);

	void HandleTag(HTMLTag *theTag);

	void GenerateBase();
	void GenerateTableData(HTMLDisplayGen &theParser, MultiLineElement *theDisplay, bool isHeader,  HTMLAlign theStartAlign);

public:
	HTMLDisplayGen();
	void SetNoBackground(bool noBackground) { mNoBackground = noBackground; }
	void SetFontSize(HTMLFontSize theSize) { mFontSize = theSize; }
	void Generate(HTMLDocument *theDocument, MultiLineElement *theDisplay, Table *theDisplayContainer);
	
	static void SetGlobalDefaultFontFace(const std::string &theFace);
};



}; // namespace WONAPI

#endif