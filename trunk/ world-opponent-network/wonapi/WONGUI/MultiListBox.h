#ifndef __WON_MULTILISTBOX_H__
#define __WON_MULTILISTBOX_H__

#include "ListBox.h"
#include "Align.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiListItem : public ListItem
{
protected:
	typedef std::vector<ListItemPtr> ItemVector;
	ItemVector mItems;

	ListItem* GetMouseItem(ListArea *theListArea, int &x);
	ListItem* GetMouseItem(ListArea *theListArea, int &x, int &y);

public:
	void InsertItem(ListItem *theItem);
	void SetItem(ListItem *theItem, int thePos);
	ListItem* GetItem(int thePos) const;

	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual void CalcDimensions(ListArea *theListArea);
	virtual void SetViewport(ListArea *theListArea, Container *theViewport);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
	virtual void SetPos(int x, int y);
	virtual bool Contains(int /*x*/, int y) { return y>=my && y<my+mHeight; }
	virtual bool CheckMouseSel(ListArea *theListArea, int x, int y);

	virtual void MouseEnter(ListArea *theListArea, int x, int y); 
	virtual void MouseExit(ListArea *theListArea);

	virtual void MouseMove(ListArea *theListArea, int x, int y);
	virtual void MouseDrag(ListArea *theListArea, int x, int y);

	virtual void MouseDown(ListArea *theListArea, int x, int y, MouseButton theButton);
	virtual void MouseUp(ListArea *theListArea, int x, int y, MouseButton theButton);

};
typedef WONAPI::SmartPtr<MultiListItem> MultiListItemPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiListArea;
class MultiListHeader : public Container
{
protected:
	struct ColumnInfo
	{
		Component *mComponent;
		ListSortType mSortType;

		ColumnInfo(Component *theComponent = NULL) : mComponent(theComponent), mSortType(ListSortType_None) { }
	};

	typedef std::vector<ColumnInfo> ColumnVector;
	ColumnVector mColumnVector;

	MultiListArea *mListArea;
	int mDragColumn;
	Component *mDragComponent;
	int mDragX;
	int mCurSortCol;
	bool mDrawSortArrow;

	Component* GetDragComponent(int x);
	virtual Component* FindChildAt(int x, int y);

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void MouseMove(int x, int y);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseDrag(int x, int y);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();
	virtual void Paint(Graphics &g);
	virtual void SizeChanged();

	int GetColumnIndex(Component *theComponent);
	ListSortType GetSortType(int theColumnIndex);

	static ListSortType GetSortTypeForButton(Component *theHeaderButton);

public:
	MultiListHeader(MultiListArea *theArea = NULL);
	void SetColumnItem(int theColumn, Component *theItem);
	void SetSortColumn(int theCol, ListSortType theType);
	void NotifyColumnChange();	

	void SetMultiListArea(MultiListArea *theListArea);
	bool GetDrawSortArrow() { return mDrawSortArrow; }
	void SetDrawSortArrow(bool draw) { mDrawSortArrow = draw; }

};
typedef WONAPI::SmartPtr<MultiListHeader> MultiListHeaderPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiListArea : public ListArea
{
protected:
	struct ColumnInfo
	{
		ScrollAreaViewportPtr mContainer;
		int mx, mWidth;
		SortFunc mSortFunc;
		unsigned char mSizeMode;
		int mPartnerColumn;
		unsigned char mHAlign;
		ColumnInfo() : mx(0), mWidth(0), mSortFunc(NULL), mSizeMode(1), mPartnerColumn(-1), mHAlign(HorzAlign_Left) { }
	};
	typedef std::vector<ColumnInfo> ColumnInfoVector;
	ColumnInfoVector mColumnInfoVector;
	SortFunc mSortColFunc;

	friend class MultiListItem;
	int mColumnPaintContext;
	int mTotalColumnWidth;
	int mSortColumn;
	int mSortColumnPrev;
	int mColumnBorderWidth;
	int mDynamicSizeColumn;
	bool mAutoGrowColumns;

	ListItem *mMouseColItem;
	int mMouseColNum;

	int mMaxTotalColumnWidth;

	friend class MultiListHeader;
	MultiListHeader *mListHeader;
	void NotifyListHeader();

	virtual void AdjustPosition(bool becauseOfScrollbar = false);
	virtual void ScrollPaint(Graphics &g);
	virtual void SizeChanged();
	virtual void NotifyMouseItemGone();

	// Protect base class methods so user doesn't accidently call them
	void InsertString(const GUIString &/*theStr*/, int /*thePos */= -1) {  }
	int GetColumnAt(int x);
	int GetColumnPos(int theCol);

	void SetColumnWidth(int theCol, int theWidth, bool fromSizeChanged);

public:
	int GetColumnPaintContext() { return mColumnPaintContext; }

public:
	MultiListArea(int theNumColumns = 1);

	// Update
	MultiListItem* InsertRow(int theRow = -1); // insert new blank row (use SetCell to fill it in)
	MultiListItem* InsertItem(MultiListItem *theItem, int theRow = -1);
	MultiListItem* UpdateItem(MultiListItem *theItem);
	MultiListItem* SetCell(int theRow, int theCol, ListItem *theCellItem);
	MultiListItem* SetString(int theRow, int theCol, const GUIString &theStr, Image *theIcon = NULL);
	void SetSortColumn(int theCol, ListSortType theType);
	void SetSortColumnFunc(int theCol, SortFunc theFunc);

	// List Info
	MultiListItem* GetItem(int thePos) { return (MultiListItem*)ListArea::GetItem(thePos); }
	MultiListItem* GetSelItem() { return (MultiListItem*)ListArea::GetSelItem(); }
	MultiListItem* GetNextSelection() { return (MultiListItem*)ListArea::GetNextSelection(); }

	int GetNumColumns() { return mColumnInfoVector.size(); }
	int GetSortColumn() { return mSortColumn; }
	int GetSortColumnPrev() { return mSortColumnPrev; }

	// Properties
	void SetNumColumns(int theNumColumns);
	void SetColumnWidth(int theCol, int theWidth) { SetColumnWidth(theCol,theWidth,false); }
	void SetColumnSizeMode(int theCol, unsigned char theMode);
	void SetColumnHAlign(int theCol, HorzAlign theAlign);
	void SetColumnBorderWidth(int theWidth) { mColumnBorderWidth = theWidth; }
	void SetDynamicSizeColumn(int theColumn) { mDynamicSizeColumn = theColumn; }
	void SetMaxTotalColumnWidth(int theMaxTotalColumnWidth) { mMaxTotalColumnWidth = theMaxTotalColumnWidth; }
	void SetAutoGrowColumns(bool autoGrow) { mAutoGrowColumns = autoGrow; }

	int GetColumnWidth(int theCol);

};
typedef WONAPI::SmartPtr<MultiListArea> MultiListAreaPtr;

}; // namespace WONAPI

#endif
