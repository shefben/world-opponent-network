#ifndef __WON_TEXTBOX_H__
#define __WON_TEXTBOX_H__

#include "Container.h"
#include "ScrollArea.h"
#include "SelectionColor.h"
#include "Align.h"

#include <deque>
#include <vector>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextLine;
class TextArea;

class TextSegmentBase : public WONAPI::RefCount
{
public:
	TextLine *mLine;
	int mAbsStartChar, mAbsEndChar;
	int mWidth, mHeight, mAscent, mx, my;
	bool mLineBreak;

public:
	TextSegmentBase();
	virtual int GetLength() { return 1; }
	virtual void CalcDimensions() { }
	virtual void PerformInstructions(TextArea * /*theTextArea*/) { }
	virtual void GetSelText(TextArea *theTextArea, GUIString &theStr);
	virtual void Paint(Graphics &/*g*/, TextArea * /*theTextArea*/) { }
	virtual int CharAt(int x) { if(x<mx+mWidth/2) return mAbsStartChar; else return mAbsEndChar; }
	virtual void Wrap(TextArea *theTextArea, TextLine *theLine);

	virtual bool IsLink() { return false; }
	virtual void Pressed() { }
	virtual bool Released() { return false; }
	virtual const GUIString& GetLinkParam() { return GUIString::EMPTY_STR; }

	virtual TextSegmentBase* GetActual() { return this; }
};
typedef WONAPI::SmartPtr<TextSegmentBase> TextSegmentBasePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextSegment : public TextSegmentBase
{
public:
	GUIString mText;
	int mColor;
	FontPtr mFont;

public:
	TextSegment();
	virtual int GetLength() { return mText.length(); }
	virtual void CalcDimensions();
	virtual void GetSelText(TextArea *theTextArea, GUIString &theStr);
	
	void Paint(Graphics &g, TextArea *theTextArea, int theStartChar, int theEndChar, int xpos, int ypos, int lineHeight);
	virtual void Paint(Graphics &g, TextArea *theTextArea);

	int CharAt(int theWidth, int theStartChar, int theEndChar);
	virtual int CharAt(int x);

	virtual void Wrap(TextArea *theTextArea, TextLine *theLine);
};
typedef WONAPI::SmartPtr<TextSegment> TextSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextSegmentPiece : public TextSegmentBase
{
public:
	TextSegment *mSeg;
	int mStartChar, mEndChar;

public:
	TextSegmentPiece(TextSegment *theSeg, int theStartChar, int theEndChar, int theWidth);
	virtual void TextSegmentPiece::Paint(Graphics &g, TextArea *theTextArea);

	virtual int CharAt(int x);
	virtual bool IsLink() { return mSeg->IsLink(); }
	virtual void Pressed() { mSeg->Pressed(); }
	virtual bool Released() { return mSeg->Released(); }
	virtual const GUIString& GetLinkParam() { return mSeg->GetLinkParam(); }

	int GetAbsStartChar() { return mSeg->mAbsStartChar + mStartChar; }
	int GetAbsEndChar() { return mSeg->mAbsStartChar + mEndChar; }
	int GetStartChar() { return mStartChar; } 
	int GetEndChar() { return mEndChar; }

	virtual TextSegmentBase* GetActual() { return mSeg; }
};
typedef WONAPI::SmartPtr<TextSegmentPiece> TextSegmentPiecePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextLink : public TextSegment
{
public:
	GUIString mLinkParam;
	bool mPressed;
	DWORD mDownColor, mNormalColor;

public:
	TextLink();

	virtual bool IsLink() { return true; }
	virtual void Pressed();
	virtual bool Released();
	virtual const GUIString& GetLinkParam() { return mLinkParam; }
};
typedef WONAPI::SmartPtr<TextLink> TextLinkPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextImage : public TextSegmentBase
{
public:
	ImagePtr mImage;
	GUIString mText;

	virtual void Paint(Graphics &g, TextArea *theTextArea);
	virtual void CalcDimensions();
	virtual void GetSelText(TextArea *theTextArea, GUIString &theStr);
};
typedef WONAPI::SmartPtr<TextImage> TextImagePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextAlignInstruction : public TextSegmentBase
{
public:
	HorzAlign mAlign;

	virtual void PerformInstructions(TextArea *theTextArea);
};
typedef WONAPI::SmartPtr<TextAlignInstruction> TextAlignInstructionPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextWidthInstruction : public TextSegmentBase 
{
public:
	int mLineShouldBeAtLeastThisWidth;

	TextWidthInstruction(int theWidth = 0) : mLineShouldBeAtLeastThisWidth(theWidth) { }
	virtual void PerformInstructions(TextArea *theTextArea);	// doesn't clip
};
typedef WONAPI::SmartPtr<TextWidthInstruction> TextWidthInstructionPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextAnchorInstruction : public TextSegmentBase 
{
public:
	GUIString mAnchorName;
	TextAnchorInstruction(const GUIString &theAnchorName) : mAnchorName(theAnchorName) { }
	virtual void PerformInstructions(TextArea *theTextArea);	
};
typedef WONAPI::SmartPtr<TextWidthInstruction> TextWidthInstructionPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextLine : public WONAPI::RefCount
{
public:
	int mx, my;
	int mWidth;
	int mHeight;
	int mBaseline;
	int mDescent;

	typedef std::list<TextSegmentBasePtr> SegmentList;
	SegmentList mSegmentList;

public:
	TextLine();
	virtual ~TextLine();
};
typedef WONAPI::SmartPtr<TextLine> TextLinePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextLinkActivatedEvent : public ComponentEvent
{
public:
	TextSegmentBasePtr mLink;

	TextLinkActivatedEvent(Component *theComponent, TextSegmentBase *theLink) : 
		ComponentEvent(theComponent, ComponentEvent_TextLinkActivated), mLink(theLink) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum TextFlags
{
	TextFlag_WordWrap			= 0x0001,
	TextFlag_AllowSelection		= 0x0002,
	TextFlag_ScrollOnBottom		= 0x0004,
	TextFlag_ShowPartialLines	= 0x0008,
	TextFlag_ScrollFullLine		= 0x0010,
	TextFlag_DelayWordWrap		= 0x0020		
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextArea : public ScrollArea
{
protected:
	typedef std::list<TextSegmentBasePtr> SegmentList;
	typedef std::deque<TextLinePtr> LineVector;

	SegmentList mSegmentList;
	LineVector mLineVector;
	int mTopLineVectorPos;
	int mMaxChars, mMaxLines;
	int mNumLines;
	bool mNeedRecalcWordWrap;
	int mDelayWordWrapTimeAccum;

	TextSegmentBase *mCurLink;
	TextSegmentBase *mLastFindSeg;
	GUIString mLinkParam;

	bool mSelecting;
	bool mBackwardsSel;
	bool mHasBeenVisible;
	int mSelStartChar, mSelEndChar;

	struct LineSearchPred
	{
		bool operator()(const TextLine *l1, const TextLine *l2) { return l1->my < l2->my; }
	};
	TextLinePtr mSearchLine;

	TextLine* AddLine(int theHeight=0, int theBaseline=0);
	void AlignLine(TextLine *theLine, HorzAlign theAlign);
	void AddSeg(TextSegmentBase *theSegment);
	void AddSegmentPrv(TextSegmentBase *theSegment);

	TextSegmentBase* FindSegment(int x, int y, bool lenient);
	LineVector::iterator FindLineAt(int theYPos);
	void FindTopLine();
	void Reset(bool justDisplay = false);
	void EnforceInvariants();
	void RemoveSegmentsUpTo(TextSegmentBase *theSegment);
	void EnforceMaxChars();
	void EnforceMaxLines();

	virtual void AdjustPosition(bool becauseOfScrollbar = false);
	bool IsLineVisible(TextLine *theLine);
	bool ShouldScrollOnBottom();

protected:
	int mTextFlags;

	FontPtr mFont;
	FontPtr mLinkFont;
	int mTextColor;
	int mLinkColor;
	SelectionColor mSelColor;

	int mWrapIndentSize;
	HorzAlign mLineAlign;

	typedef std::map<GUIString,int> AnchorMap;
	AnchorMap mAnchorMap;

	friend class TextSegmentBase;
	friend class TextSegment;
	friend class TextImage;
	friend class TextAlignInstruction;
	friend class TextSpaceInstruction;

public:
	virtual void SizeChanged();
	virtual void ScrollPaint(Graphics &g);
	virtual void CheckSel(int x, int y, bool newStart);
	virtual void MouseEnter(int x, int y);
	virtual void MouseMove(int x, int y);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();
	virtual void MouseDrag(int x, int y);
	virtual bool TimerEvent(int theDelta);

	void CopySelection();
	virtual bool KeyChar(int theKey);
	virtual bool KeyDown(int theKey);

	void RecalcWordWrap(bool wasDelayed);
	virtual void NotifyRootVisibilityChange(bool isVisible);

public:
	TextArea();
	virtual ~TextArea();

	// Update
	void AddSegment(TextSegmentBase *theSegment);
	void AddSegment(const GUIString &theText, bool lineBreak = false);
	void AddSegment(const GUIString &theText, int theColor, bool lineBreak = false);
	void AddSegment(const GUIString &theText, int theColor, Font *theFont, bool lineBreak = false);
	void AddFormatedText(const GUIString &theText, bool lineBreak = false); // adds line breaks in place of carriage returns
	void AddLink(const GUIString &theText, const GUIString &theLinkParam, bool lineBreak = false);
	void AddImage(Image *theImage, const GUIString &theCopyText, bool lineBreak = false);
	void LineBreak(int theHeight = 0);
	void Clear() { Reset(); }

	// Properties
	void SetTextFlags(int theFlags, bool on);
	void SetTextColor(int theColor) { mTextColor = theColor; }
	void SetSelectionColor(const SelectionColor &theSelColor) { mSelColor = theSelColor; }
	void SetFont(Font *theFont) { mFont = theFont; }
	void SetLinkFont(Font *theFont) { mLinkFont = theFont; }

	void SetMaxChars(int theMaxChars);
	void SetMaxLines(int theMaxLines);
	void SetWrapIndentSize(int theIndentSize) { mWrapIndentSize = theIndentSize; }
	void SetLineAlignment(HorzAlign theAlignment);

	bool TextFlagSet(int theFlags) { return mTextFlags&theFlags?true:false; }
	const GUIString& GetLinkParam() { return mLinkParam; }
	int GetNumLines() { return mLineVector.size(); }
	Font* GetTextFont() { return mFont; }
	Font* GetLinkFont() { return mLinkFont; }
	int GetTextColor() { return mTextColor; }

	// Anchors
	void AddAnchor(const GUIString& theName, int theLineNum);
	bool GotoAnchor(const GUIString& theName);


};
typedef WONAPI::SmartPtr<TextArea> TextAreaPtr;

}; // namespace WONAPI

#endif
