#ifndef __TABLE_H__
#define __TABLE_H__

#include "ScrollArea.h"
#include "Align.h"
#include <vector>

namespace WONAPI
{

class MultiLineBuilder;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LineSegmentFlags
{
	LineSegmentFlag_FloatLeft			= 0x0001,
	LineSegmentFlag_FloatRight			= 0x0002,
	LineSegmentFlag_PreLineBreak		= 0x0004,
	LineSegmentFlag_PostLineBreak		= 0x0008,
	LineSegmentFlag_BreakClearLeft		= 0x0010,
	LineSegmentFlag_BreakClearRight		= 0x0020
};

class Table;
class LineSegment;
typedef SmartPtr<LineSegment> LineSegmentPtr;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ElementPaintInfo
{
	int mClipX, mClipY, mClipWidth, mClipHeight;
	int mLineY, mLineHeight;
	int mStartSelSegment, mEndSelSegment;
	int mStartSelSegmentPos, mEndSelSegmentPos;

	int ClipLeft() { return mClipX; }
	int ClipTop() { return mClipY; }
	int ClipRight() { return mClipX + mClipWidth; }
	int ClipBottom() { return mClipY + mClipHeight; }

	ElementPaintInfo() : mLineY(-1), mLineHeight(-1) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ElementSelectionInfo
{
	int mStartSelSegment, mEndSelSegment;
	int mStartSelSegmentPos, mEndSelSegmentPos;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ElementFindInfo
{
	int mStartSelSegment, mEndSelSegment;
	int mStartSelSegmentPos, mEndSelSegmentPos;

	GUIString mFindText;
	bool mCaseSensitive;
	bool mFindPrev;
	int mCurFoundLength;

	int mFoundStartSegment, mFoundEndSegment;
	int mFoundStartChar, mFoundEndChar;
	int mFoundX, mFoundY;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LineSegment : public RefCount
{
public:
	int mStartSelIndex;
	int mAbsX, mAbsY;

	virtual int GetWidth()			{ return 0; }
	virtual int GetMinWidth()		{ return GetWidth(); }
	virtual int GetMaxWidth()		{ return GetWidth(); }
	virtual int GetHeight()			{ return 0; }
	virtual int GetAscent()			{ return GetHeight(); }
	virtual int GetFirstWrapWidth() { return GetWidth(); }
	virtual char GetVertAlign()		{ return 0; } 
	virtual int GetStartSelIndex() { return mStartSelIndex; }
	virtual int GetEndSelIndex() { return mStartSelIndex; }

	static Table* mTableContext;
	static int mCounter;
	static ImagePtr mSelectImageMask;

	bool DrawSelection1(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
	void DrawSelection2(Graphics &g, int x, int y);

public:
	virtual void MouseEnter(int /*x*/, int /*y*/) { }
	virtual void MouseExit() { }
	virtual void MouseDown(int /*x*/, int /*y*/, MouseButton /*theButton*/) { }
	virtual void MouseUp(int /*x*/, int /*y*/, MouseButton /*theButton*/) { }
	virtual void MouseMove(int /*x*/, int /*y*/) { }
	virtual void MouseDrag(int /*x*/, int /*y*/) { }
	virtual void LinkDown() { }
	virtual void LinkUp() { }
	virtual bool CanStartSelection() { return true; }
	virtual int GetSelChar(int x, int y);
	virtual bool GetSelCharPos(int &/*x*/, int &/*y*/, int /*theSelIndex*/, int /*theSelChar*/) { return false; }
	virtual void GetSelText(GUIString &theText, ElementSelectionInfo &theInfo);
	virtual bool FindStr(ElementFindInfo &/*theInfo*/) { return false; }

	virtual void SetSelIndex() { mStartSelIndex = mCounter++; }
	virtual LineSegment* GetSegmentAt(int &/*x*/, int &/*y*/, bool /*lenient*/) { return this; }
	virtual bool IsInvalid() { return false; }
	virtual bool AllowModalInput() { return false; }

public:
	virtual bool PerformInstructions(MultiLineBuilder * /*theContainer*/) { return true; }
	virtual void Paint(Graphics &/*g*/, int /*x*/, int /*y*/, ElementPaintInfo &/*theInfo*/) { }
	virtual void PaintSelection(Graphics &/*g*/, int /*x*/, int /*y*/, ElementPaintInfo &/*theInfo*/) { }
	virtual void Wrap(MultiLineBuilder *theContainer, bool needSegmentOnFirstLine);
	virtual void SetMaxWidth(int /*theWidth*/) { }
	virtual void SetViewport(ScrollAreaViewport *theViewport, int x, int y);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentLineSegment : public LineSegment
{
public:
	ComponentPtr mComponent;

public:
	virtual void MouseEnter(int x, int y) { mComponent->MouseEnter(x,y); }
	virtual void MouseExit() { mComponent->MouseExit(); }
	virtual void MouseDown(int x, int y, MouseButton theButton) { mComponent->MouseDown(x,y,theButton); }
	virtual void MouseUp(int x, int y, MouseButton theButton) { mComponent->MouseUp(x,y,theButton); }
	virtual void MouseMove(int x, int y) { mComponent->MouseMove(x,y); }
	virtual void MouseDrag(int x, int y) { mComponent->MouseDrag(x,y); }
	virtual bool IsInvalid() { return mComponent->IsInvalid();  }
	virtual void SetViewport(ScrollAreaViewport *theViewport, int x, int y);
	virtual bool CanStartSelection() { return false; }
	virtual bool AllowModalInput() { return mComponent->ComponentFlagSet(ComponentFlag_AllowModalInput); }

public:
	ComponentLineSegment(Component *theComponent) : mComponent(theComponent) { }

	virtual int GetWidth() { return mComponent->Width(); }
	virtual int GetHeight() { return mComponent->Height(); }
	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<ComponentLineSegment> ComponentLineSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AlignLineSegment : public LineSegment
{
public:
	HorzAlign mAlign;

	AlignLineSegment(HorzAlign theAlign) : mAlign(theAlign) { }
	virtual bool PerformInstructions(MultiLineBuilder *theContainer);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MarginLineSegment : public LineSegment
{
public:
	int mLeftMargin;
	int mNextLeftMargin;

	MarginLineSegment(int leftMargin, int nextLeftMargin) : mLeftMargin(leftMargin), mNextLeftMargin(nextLeftMargin) { }
	virtual bool PerformInstructions(MultiLineBuilder *theContainer);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AnchorLineSegment : public LineSegment
{
public:
	GUIString mName;

	AnchorLineSegment(const GUIString &theName) : mName(theName) { }
	virtual bool PerformInstructions(MultiLineBuilder *theContainer);
	virtual void SetViewport(ScrollAreaViewport *theViewport, int x, int y);
	virtual char GetVertAlign()		{ return 2; } 
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextLineSegment : public LineSegment
{
public:
	GUIString mText;
	int mColor;
	FontPtr mFont;
	int mWidth, mMinWidth;

public:
	TextLineSegment() : mWidth(0), mMinWidth(0) { }
	TextLineSegment(const GUIString &theText, int theColor, Font *theFont) : mWidth(0), mMinWidth(0), mText(theText), mColor(theColor), mFont(theFont) { }

	virtual void MouseEnter(int x, int y);
	virtual int GetWidth();
	virtual int GetMinWidth(); 
	virtual int GetHeight()		{ return mFont->GetHeight(); } 
	virtual int GetAscent()		{ return mFont->GetAscent(); }
	virtual int GetFirstWrapWidth();
	virtual int GetSelChar(int x, int y);
	int GetSelChar(int x, int y, int theStartChar, int theEndChar);
	virtual bool GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar);
	virtual void GetSelText(GUIString &theText, ElementSelectionInfo &theInfo);
	virtual bool CanStartSelection() { return true; }

	bool FindPartial(ElementFindInfo &theInfo, int startChar, int endChar);
	virtual bool FindStr(ElementFindInfo &theInfo);

	virtual void Wrap(MultiLineBuilder *theContainer, bool needSegmentOnFirstLine);
	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
	virtual void PaintSelection(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
	void Paint(Graphics &g, int x, int y, int theSelIndex, int startChar, int endChar, int theWidth, int theHeight, ElementPaintInfo &theInfo);
	void PaintSelection(Graphics &g, int x, int y, int theSelIndex, int startChar, int endChar, int theWidth, int theHeight, ElementPaintInfo &theInfo);
};
typedef SmartPtr<TextLineSegment> TextLineSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextPieceLineSegment : public LineSegment
{
public:
	TextLineSegment *mSegment;
	int mStartChar, mEndChar;
	int mWidth;

public:
	TextPieceLineSegment(TextLineSegment *theSegment, int theStartChar, int theEndChar, int theWidth) :
	  mSegment(theSegment), mStartChar(theStartChar), mEndChar(theEndChar), mWidth(theWidth) { mStartSelIndex = theSegment->mStartSelIndex; }

	virtual void MouseEnter(int x, int y) { mSegment->MouseEnter(x,y); }
	virtual void MouseExit() { mSegment->MouseExit(); }
	virtual void MouseDown(int x, int y, MouseButton theButton) { mSegment->MouseDown(x,y,theButton); }
	virtual void MouseUp(int x, int y, MouseButton theButton) { mSegment->MouseUp(x,y,theButton); }
	virtual void MouseMove(int x, int y) { mSegment->MouseMove(x,y); }
	virtual void MouseDrag(int x, int y) { mSegment->MouseDrag(x,y); }
	virtual bool IsInvalid() { return mSegment->IsInvalid(); }
	virtual int GetSelChar(int x, int y);
	virtual bool GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar);
	virtual bool CanStartSelection() { return mSegment->CanStartSelection(); }

	virtual int GetWidth() { return mWidth; }
	virtual int GetHeight() { return mSegment->GetHeight(); }
	virtual int GetAscent() { return mSegment->GetAscent(); }

	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
	virtual void PaintSelection(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<TextPieceLineSegment> TextPieceLineSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextLinkParam : public RefCount
{
public:
	GUIString mLinkParam;
	typedef std::list<LineSegment*> LinkList;
	LinkList mLinkList;
	bool mLinkIsDown;
	bool mLaunchBrowser;

	TextLinkParam(const GUIString &theLinkParam, bool launchBrowser) : mLinkParam(theLinkParam), 
		mLaunchBrowser(launchBrowser), mLinkIsDown(false) { }
	
	void AddLink(LineSegment *theSegment) { mLinkList.push_back(theSegment); }
	void LinkDown();
	bool LinkUp();
};
typedef SmartPtr<TextLinkParam> TextLinkParamPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HyperLinkEvent : public ComponentEvent
{
public:
	TextLinkParamPtr mLinkParam;

	HyperLinkEvent(Component *theComponent, int theType, TextLinkParam *theParam) : 
		ComponentEvent(theComponent, theType), mLinkParam(theParam) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextLinkLineSegment : public TextLineSegment
{
public:
	DWORD mNormalColor, mDownColor;
	TextLinkParamPtr mLinkParam;

public:
	TextLinkLineSegment(TextLinkParam *theParam, const GUIString &theText, int theColor, int theDownColor, Font *theFont) : 
	  mLinkParam(theParam), mNormalColor(theColor), mDownColor(theDownColor), TextLineSegment(theText,theColor,theFont) { mLinkParam->AddLink(this); if(mText.length()==1 && isspace(mText.at(0))) mText.erase(); }

	virtual void LinkDown();
	virtual void LinkUp();
	virtual void MouseEnter(int x, int y);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();
	virtual bool CanStartSelection() { return false; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LineBreakSegment : public LineSegment
{
public:
	int mHeight;

public:
	LineBreakSegment(int theHeight) : mHeight(theHeight) { }

	virtual int GetHeight() { return mHeight; }
	virtual void GetSelText(GUIString &theText, ElementSelectionInfo &theInfo);
};
typedef SmartPtr<LineBreakSegment> LineBreakSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HorizontalRuleSegment : public LineSegment
{
public:
	int mWidthSpec, mWidth, mHeight;
	bool mNoShade;

	HorizontalRuleSegment() : mWidthSpec(0), mWidth(0), mHeight(1), mNoShade(false) { }

	virtual int GetAscent() { return GetHeight()/2; }
	virtual int GetHeight() { return (mHeight + 14); }
	virtual int GetWidth() { return mWidth; }
	virtual void SetMaxWidth(int theWidth);
	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
	virtual void GetSelText(GUIString &theText, ElementSelectionInfo &theInfo);
};
typedef SmartPtr<HorizontalRuleSegment> HorizontalRuleSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageLineSegment : public LineSegment
{
public:
	ImagePtr mImage;
	int mHSpace, mVSpace, mBorder, mBorderColor;
	char mAlign;

public:
	ImageLineSegment(Image *theImage = NULL) : mImage(theImage), mHSpace(5), mVSpace(0), mAlign(0), mBorder(0), mBorderColor(0) { }

	virtual int GetWidth()			{ return mImage->GetWidth() + 2*mHSpace + 2*mBorder; }
	virtual int GetHeight()			{ return mImage->GetHeight() + 2*mVSpace + 2*mBorder; }
	virtual char GetVertAlign()		{ return mAlign; } 

	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<ImageLineSegment> ImageLineSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageMap : public RefCount
{
public:
	struct ShapeLink
	{
		TextLinkParamPtr mLinkParam;
		WONRectangle mShape;

		ShapeLink(TextLinkParam *theLink, WONRectangle &theShape) : mLinkParam(theLink), mShape(theShape) { }
	};
	typedef std::list<ShapeLink> ShapeList;
	ShapeList mShapeList;

	void AddLink(TextLinkParam *theLink, WONRectangle &theShape) { mShapeList.push_back(ShapeLink(theLink,theShape)); }
	TextLinkParam* GetLink(int x, int y);
};
typedef SmartPtr<ImageMap> ImageMapPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageLinkLineSegment : public ImageLineSegment
{
public:
	DWORD mNormalColor, mDownColor;
	TextLinkParamPtr mCurLinkParam;
	TextLinkParamPtr mAnchorLinkParam;
	ImageMapPtr mImageMap;

public:
	ImageLinkLineSegment(TextLinkParam *theParam, int theColor, int theDownColor, Image *theImage) : 
		mAnchorLinkParam(theParam), mCurLinkParam(theParam), mDownColor(theDownColor), mNormalColor(theColor), ImageLineSegment(theImage) { if(theParam!=NULL) theParam->AddLink(this); }

	virtual void LinkDown();
	virtual void LinkUp();
	virtual void MouseEnter(int x, int y);
	virtual void MouseMove(int x, int y);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();
	virtual bool CanStartSelection() { return false; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScaledImageLineSegment : public ImageLineSegment
{
public:
	int mWidth, mHeight;

public:
	ScaledImageLineSegment(Image *theImage, int theWidth, int theHeight) : ImageLineSegment(theImage), mWidth(theWidth), mHeight(theHeight) { }

	virtual int GetWidth() { return mWidth + 2*mHSpace; }
	virtual int GetHeight() { return mHeight + 2*mVSpace; }
	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<ScaledImageLineSegment> ScaledImageLineSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BulletElement : public LineSegment
{
public:
	int mWidth, mHeight;
	int mColor, mType;

public:
	BulletElement(int theColor = 0, int theType = -1, int theWidth = 5, int theHeight = 5) : mColor(theColor), mType(theType), mWidth(theWidth), mHeight(theHeight) { }

	virtual int GetWidth()		{ return mType>=0?0:mWidth+10; }
	virtual int GetHeight()		{ return mHeight+2; }

	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<BulletElement> BulletElementPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class OrderedListElement : public LineSegment
{
public:
	FontPtr mFont;
	int mColor, mWidth;
	GUIString mText;

public:
	OrderedListElement(int theColor, Font* theFont, const GUIString &theText) : mColor(theColor), mFont(theFont), mText(theText)
		{ mWidth = theFont->GetStringWidth(mText); }

	virtual int GetWidth()		{ return 0; }
	virtual int GetHeight()		{ return mFont->GetHeight(); }
	virtual int GetAscent()		{ return mFont->GetAscent(); }
	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<OrderedListElement> OrderedListElementPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LineElement : public LineSegment
{
public:
	typedef std::list<LineSegmentPtr> SegmentList;
	SegmentList mSegmentList;
	bool mIsGlueLine;

	int mLeftMargin;
	int mBaseline;
	int mWidth, mHeight;
	int my;

public:
	virtual int GetWidth() { return mWidth; }
	virtual int GetHeight() { return mHeight; }
	virtual int GetAscent() { return mBaseline; }

	virtual LineSegment* GetSegmentAt(int &x, int &y, bool lenient);
	virtual void SetViewport(ScrollAreaViewport *theViewport, int x, int y);
	virtual void SetSelIndex();
	virtual void GetSelText(GUIString &theText, ElementSelectionInfo &theInfo);
	virtual bool GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar);
	virtual bool FindStr(ElementFindInfo &theInfo);

	virtual int GetEndSelIndex();

public:
	LineElement();
	void AddSegment(LineSegment *theSegment);
	bool IsEmpty() { return mSegmentList.empty(); }
	void CalcDimension();
	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
	virtual void PaintSelection(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<LineElement> LineElementPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LineBuilder
{
public:
	int mMaxAscent, mMaxDescent, mTopAlignHeight, mWidth;
	LineElementPtr mLine;

	void Clear() { mLine = NULL; }
	void StartBuild(LineElement *theElement);
	void AddSegment(LineSegment *theSegment);
	void FinishBuild();
	bool IsStarted() { return mLine.get()!=NULL; }
	bool IsEmpty() { return mLine.get()?mLine->IsEmpty():true; }

	int GetHeight() { if(mMaxAscent + mMaxDescent >= mTopAlignHeight) return mMaxAscent+mMaxDescent; else return mTopAlignHeight; }
	int GetWidth() { return mWidth; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct FloatingSegment 
{
public:
	LineSegmentPtr mSegment;
	int mx, my;

public:
	FloatingSegment(LineSegment *theSegment = NULL, int x = 0, int y = 0) : mSegment(theSegment), mx(x), my(y) { }

	virtual int GetWidth()			{ return mSegment->GetWidth(); }
	virtual int GetHeight()			{ return mSegment->GetHeight(); }
};
typedef SmartPtr<FloatingSegment> FloatingSegmentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef std::vector<FloatingSegment> FloatingVector;

struct SegmentInfo
{
	LineSegmentPtr mSegment;
	unsigned short mFlags;

	SegmentInfo(LineSegment *theSegment = NULL, unsigned short theFlags = 0) : 
		mSegment(theSegment), mFlags(theFlags) { }
};
typedef std::list<SegmentInfo> SegmentList;
typedef std::vector<LineElementPtr> LineVector;

class MultiLineBuilder
{
protected:
	friend class MultiLineElement;

	LineVector *mLineVector;
	SegmentList *mSegmentList;
	int mWidth, mHeight;
	int mSpecifiedWidth;
	int mLeftMargin, mRightMargin;
	int mMaxLeftHeight, mMaxRightHeight;
	int mLeftIndent, mRightIndent;
	HorzAlign mAlignment;
	LineBuilder mCurLine;
	int mCurSelIndex;

	FloatingVector *mLeftItems;
	FloatingVector *mRightItems;

	bool mHaveDeferredItems;
	typedef std::list<LineSegment*> LineSegmentList;
	LineSegmentList mDeferredLeftItems;
	LineSegmentList mDeferredRightItems;
	bool FlushDeferredItems();

	typedef std::list<FloatingSegment> FloatingList;
	FloatingList mCurLeftItems;
	FloatingList mCurRightItems;

public:
	MultiLineBuilder();
	void AddSegments(SegmentList *theSegmentList, LineVector *theLineVector, 
		FloatingVector *theLeftItems, FloatingVector *theRightItems, int theMaxWidth, int theStartSelIndex);

	void AlignCurLine();
	void CalcMargins();
	void AddSegmentToCurLine(LineSegment *theSegment);
	void AddFloatingSegment(LineSegment *theSegment, bool left);
	LineElement* AddLine(bool clearLeft = false, bool clearRight = false);
	LineBuilder* CurLine() { return &mCurLine; }
//	int GetCurMaxLineWidth() { return mSpecifiedWidth - mLeftMargin - mRightMargin; }
	int GetCurMaxLineWidth() { return mRightMargin - mLeftMargin; }// - mLeftIndent - mRightIndent; }
	void SetLineAlignment(HorzAlign theAlign) { mAlignment = theAlign; }
	void SetIndentSize(int theLeftIndent, int theRightIndent);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiLineElement : public LineSegment
{
protected:
	FloatingVector mFloatingItems[2];
	LineVector mLineVector;
	SegmentList mSegmentList;

	LineElementPtr mSearchLine;
	struct LineSearchPred
	{
		bool operator()(const LineElement *l1, const LineElement *l2) { return l1->my < l2->my; }
	};
	LineVector::iterator FindLineAt(int theYPos);

	struct FloatSearchPred
	{
		bool operator()(const FloatingSegment &l1, const FloatingSegment &l2) { return l1.my < l2.my; }
	};
	FloatingSegment* FindFloatingSegmentAt(int x, int y, bool left);


	int mWidth, mHeight;
	int mMinWidth, mCurLineWidth, mMaxWidth;

public:
	virtual int GetWidth() { return mWidth; }
	virtual int GetHeight() { return mHeight; }
	virtual LineSegment* GetSegmentAt(int &x, int &y, bool lenient);
	virtual void SetViewport(ScrollAreaViewport *theViewport, int x, int y);
	virtual void GetSelText(GUIString &theText, ElementSelectionInfo &theInfo);
	virtual bool GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar);
	virtual bool FindStr(ElementFindInfo &theInfo);

	virtual void SetSelIndex();
	virtual int GetEndSelIndex();

	void CalcMinMaxWidths();
	virtual int GetMinWidth();
	virtual int GetMaxWidth();
	bool IsLastLineEmpty();

	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
	virtual void SetMaxWidth(int theMaxWidth);

public:
	MultiLineElement();
	void AddSegment(LineSegment *theSegment, unsigned short theFlags = 0);

};
typedef SmartPtr<MultiLineElement> MultiLineElementPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TableElement : public LineSegment
{
public:

	struct ColumnInfo
	{
		int mx;
		int mWeight;
		int mWidth;
		int mMinWidth;
		int mMaxWidth;
		int mColumnNum;
		ColumnInfo(int theColumnNum = 0) : mColumnNum(theColumnNum), mWidth(0), mMinWidth(0), mMaxWidth(0), mWeight(0) { }

		void SetWidthToMin() { mWidth = mMinWidth; }
		void SetWidthToMax() { mWidth = mMaxWidth; }
	};
	struct RowInfo;

	struct CellInfo
	{
		MultiLineElementPtr mContents;
		int mColumnSpan, mRowSpan;
		int mBGColor;
		int mWidth, mHeight;
		char mHAlign, mVAlign;
		bool mNoWrap;

		CellInfo() : mContents(new MultiLineElement), mColumnSpan(1), mRowSpan(1), mBGColor(-1), mHAlign(-1), mVAlign(-1), mWidth(0), mHeight(0), mNoWrap(false)  { }
	};
	typedef std::vector<CellInfo> CellRow;

	struct CellInfoEx
	{
		CellInfo *mCellInfo;
		int mBorderX, mBorderY, mBorderWidth, mBorderHeight;
		int mCellX, mCellY;
	};

	struct RowInfo
	{
		int my;
		int mHeight;
		int mRowNum;
		int mBGColor;
		char mHAlign, mVAlign;
		CellRow mRow;
		RowInfo(int theRowNum = 0) : mRowNum(theRowNum), mHeight(0), mBGColor(-1), mHAlign(0), mVAlign(1) { }
	};

	typedef std::vector<ColumnInfo> ColumnVector;
	typedef std::vector<RowInfo> RowVector;

	ColumnVector mColumnVector;
	RowVector mRowVector;
	int mEndSelIndex;

	struct RowSearchPred
	{
		bool operator()(const RowInfo &l1, const RowInfo &l2) { return l1.my < l2.my; }
	};
	RowInfo mSearchRow;
	int FindRowAt(int theYPos);

	int mCurRow, mCurCol;
	MultiLineElement* mCurCell;

	bool GetCellInfoEx(CellInfoEx &theInfo, int theRow, int theCol, RowInfo &theRowInfo, bool seekUp=false, bool seekLeft=false);
	CellInfo* GetCellInfoAt(int theRow, int theCol);
	CellInfo* GetCellInfoAt(RowInfo &theInfo, int theCol);
	MultiLineElement* GetCellAt(int theRow, int theCol);
	MultiLineElement* GetCellAt(RowInfo &theInfo, int theCol);
	int mWidth, mHeight;
	int mMinWidth, mMaxWidth;
	int mCellSpacing, mCellPadding, mBorder, mBGColor, mWidthSpec;

	void CalcRowHeights();
	void CalcColumnWidths(int theWidth);

	virtual void SetViewport(ScrollAreaViewport *theViewport, int x, int y);
	virtual LineSegment* GetSegmentAt(int &x, int &y, bool lenient);
	virtual void GetSelText(GUIString &theText, ElementSelectionInfo &theInfo);
	virtual bool FindStr(ElementFindInfo &theInfo);

	virtual void SetSelIndex();
	virtual int GetEndSelIndex() { return mEndSelIndex; }

public:
	TableElement();
	void SetSize(int numRows, int numCols);
	void EnsureNumRows(int numRows);
	void EnsureNumCols(RowInfo &theRow, int numCols);
	void EnsureNumCols(int numCols);

	void Rewind();
	MultiLineElement* NextCell();
	int GetCurRow() { return mCurRow; }
	int GetCurColumn() { return mCurCol; }
	HorzAlign GetCurRowAlign();

	void StartTraversal();
	void NextRow();
	CellInfo* NextColumn();
	
	void AddSegment(LineSegment *theSegment, unsigned short theFlags = 0);
	void SetRowBGColor(int theColor);
	void SetCellSpan(int theRowSpan, int theColumnSpan, bool truncate = false);
	void SetRowAlign(int theHAlign, int theVAlign);
	void SetCellAlign(int theHAlign, int theVAlign);
	void SetColumnWidth(int theWidthSpec);
	void SetCellBGColor(int theColor);
	void CalcMinMaxWidths();
	void AssignColumnWidths();
	virtual void SetMaxWidth(int theWidth);

	virtual int GetWidth() { return mWidth; }
	virtual int GetHeight() { return mHeight; }
	virtual int GetMinWidth(); 
	virtual int GetMaxWidth();

	void PaintCell(Graphics &g, int x, int y, int theRow, int theCol, RowInfo &theRowInfo, ElementPaintInfo &thePaintInfo, bool seekUp);
	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<TableElement> TableElementPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Table : public ScrollArea
{
public:
	LineSegmentPtr mContents;
	LineSegment* mMouseSegment;
	int mStartSelSegment, mEndSelSegment;
	int mStartSelSegmentPos, mEndSelSegmentPos;
	bool mBackwardsSel, mSelecting;

	int mMouseSegmentDX, mMouseSegmentDY;

	struct AnchorCoordinate
	{
		int mx,my;

		AnchorCoordinate(int x = 0, int  y = 0) : mx(x), my(y) { }
	};

	typedef std::map<GUIString,AnchorCoordinate,GUIStringLessNoCase> AnchorMap;
	AnchorMap mAnchorMap;

public:
	virtual void ScrollPaint(Graphics &g);
	virtual void ScrollSizeChanged();	
	void SetMaxWidth(bool adjustPosition = true);
	void CopySelection();

	void MouseAt(int &x, int &y, bool mouseDown = false);
	virtual void MouseEnter(int x, int y);
	virtual void MouseExit();
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseMove(int x, int y);
	virtual void MouseDrag(int x, int y);
	virtual bool TimerEvent(int theDelta);
	virtual bool KeyChar(int theKey);
	virtual bool KeyDown(int theKey);

	virtual Component* WMFindChildAt(int x, int y);

public:
	Table(LineSegment *theContents = NULL);
	void SetContents(LineSegment *theContents);
	LineSegment* GetContents() { return mContents; }

	bool FindStr(const GUIString &theStr, bool next = true, bool caseSensitive = false);
	void AddAnchor(const GUIString &theName, int x, int y);
	bool GotoAnchor(const GUIString &theName);
};
typedef SmartPtr<Table> TablePtr;

}; // namespace WONAPI
#endif