#ifndef __WON_LISTBOX_H__
#define __WON_LISTBOX_H__

#include "Component.h"
#include "ScrollArea.h"
#include "SelectionColor.h"
#include <vector>

namespace WONAPI
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ListArea;
class ListItem : public WONAPI::RefCount
{
protected:
	virtual ~ListItem() { }

public:
	bool mSelected;
	bool mSelectable;
	int my, mWidth, mHeight;

	virtual void Paint(Graphics &/*g*/, ListArea * /*theListArea*/) { }
	virtual void CalcDimensions(ListArea * /*theListArea*/) { }
	virtual void SetViewport(ListArea * /*theListArea*/, Container * /*theViewport*/) { }
	virtual int Compare(const ListItem *theItem, ListArea * /*theListArea*/) const { return this < theItem; }
	virtual bool CheckMouseSel(ListArea * /*theListArea*/, int /*x*/, int /*y*/) { return mSelectable; }
	virtual void SetPos(int /*x*/, int y) { my = y; }
	virtual bool Contains(int /*x*/, int y) { return y>=my && y<my+mHeight; }

	virtual void MouseEnter(ListArea * /*theListArea*/, int /*x*/, int /*y*/) { }
	virtual void MouseExit(ListArea * /*theListArea*/) { }

	virtual void MouseMove(ListArea * /*theListArea*/, int /*x*/, int /*y*/) { }
	virtual void MouseDrag(ListArea * /*theListArea*/, int /*x*/, int /*y*/) { }

	virtual void MouseDown(ListArea * /*theListArea*/, int /*x*/, int /*y*/, MouseButton /*theButton*/) { }
	virtual void MouseUp(ListArea * /*theListArea*/, int /*x*/, int /*y*/, MouseButton /*theButton*/) { }


public:
	ListItem();
};
typedef WONAPI::SmartPtr<ListItem> ListItemPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextListItem : public ListItem
{
protected:
	virtual ~TextListItem() { }

public:
	GUIString mText;
	int mColor;
	FontPtr mFont;
	ImagePtr mIcon;

public:
	TextListItem() : mColor(-1) { };
	TextListItem(const GUIString &theStr) : mText(theStr), mColor(-1) { }
	TextListItem(Image *theIcon) : mColor(-1), mIcon(theIcon) { }
	TextListItem(const GUIString &theStr, Image *theIcon) : mText(theStr), mIcon(theIcon), mColor(-1) { } 
	TextListItem(const GUIString &theStr, Font *theFont) : mText(theStr), mFont(theFont), mColor(-1) { } 
	TextListItem(const GUIString &theStr, DWORD theColor) : mText(theStr), mColor(theColor) { }
	TextListItem(const GUIString &theStr, Font *theFont, DWORD theColor, Image *theIcon = NULL)  : mText(theStr), mColor(theColor), mFont(theFont), mIcon(theIcon) { }

	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual void CalcDimensions(ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
};
typedef WONAPI::SmartPtr<TextListItem> TextListItemPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentListItem : public ListItem
{
protected:
	virtual ~ComponentListItem() { }

public:
	ComponentPtr mComponent;
	bool mIsSizable;


	virtual void CalcDimensions(ListArea *theListArea);
	virtual void SetViewport(ListArea *theListArea, Container *theViewport);
	virtual void SetPos(int x, int y);
	virtual bool Contains(int x, int y);
	virtual void Paint(Graphics &g, ListArea *theListArea);
	void CheckInvalid(ListArea *theListArea);

	virtual void MouseEnter(ListArea *theListArea, int x, int y) { mComponent->MouseEnter(x,y); CheckInvalid(theListArea); }
	virtual void MouseExit(ListArea *theListArea) { mComponent->MouseExit(); CheckInvalid(theListArea); }

	virtual void MouseMove(ListArea *theListArea, int x, int y) { mComponent->MouseMove(x,y); CheckInvalid(theListArea); }
	virtual void MouseDrag(ListArea *theListArea, int x, int y) { mComponent->MouseDrag(x,y); CheckInvalid(theListArea); }

	virtual void MouseDown(ListArea *theListArea, int x, int y, MouseButton theButton) { mComponent->MouseDown(x,y,theButton); CheckInvalid(theListArea); }
	virtual void MouseUp(ListArea *theListArea, int x, int y, MouseButton theButton) { mComponent->MouseUp(x,y,theButton); CheckInvalid(theListArea); }

public:
	ComponentListItem(Component *theComponent, bool isSizable = false, bool isSelectable = false) : mComponent(theComponent), mIsSizable(isSizable) { mSelectable = isSelectable; }
};
typedef WONAPI::SmartPtr<ComponentListItem> ComponentListItemPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ListItemClickedEvent : public ComponentEvent
{
public:
	MouseButton mButton;
	bool mWasDoubleClick;
	ListItemPtr mItem;
	int mItemX, mItemY;

public:
	ListItemClickedEvent(Component *theList, ListItem *theItem, MouseButton theButton, bool wasDoubleClick, bool wasUpClick, int mx, int my) : 
	  ComponentEvent(theList, wasUpClick?ComponentEvent_ListItemUpClicked:ComponentEvent_ListItemClicked), 
		  mItem(theItem), mButton(theButton), mWasDoubleClick(wasDoubleClick), mItemX(mx), mItemY(my) { }

	 bool IsSelectedItem();
};
typedef SmartPtr<ListItemClickedEvent> ListItemClickedEventPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ListSortType
{
	ListSortType_Ascending,
	ListSortType_Descending,
	ListSortType_None
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ListFlags
{
	ListFlag_ScrollOnBottom				= 0x0001,
	ListFlag_SelOver					= 0x0002,
	ListFlag_SelOnlyWhenOver			= 0x0004,
	ListFlag_ToggleSel					= 0x0008,
	ListFlag_FullRowSelect				= 0x0010,
	ListFlag_HilightIcons				= 0x0020,
	ListFlag_ScrollFullLine				= 0x0040,
	ListFlag_ShowPartialLines			= 0x0080,
	ListFlag_ClickEventOnMouseUp		= 0x0100,
	ListFlag_ItemWidthCheckOnSel		= 0x0200,
	ListFlag_MultiSelect				= 0x0400,
	ListFlag_KeyMoveWrapAround			= 0x0800,
	ListFlag_NoAutoAdjustScrollPos		= 0x1000,
	ListFlag_DoListItemToolTip			= 0x2000
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ListAreaToolTip : public Component
{
public:
	ListArea *mListArea;
	ListItemPtr mItem;

public:
	ListAreaToolTip(ListArea *theListArea);
	virtual void Paint(Graphics &g);

	void DoTip(ListItem *theItem);
	void EndTip();
};
typedef SmartPtr<ListAreaToolTip> ListAreaToolTipPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ListArea : public ScrollArea
{
protected:
	virtual ~ListArea();

	typedef std::vector<ListItemPtr> LineVector;
	LineVector mLineVector;
	int mTopLineVectorPos;

	int mListFlags;

	int mDoingMultiChange;
	int mPaintColumnWidth;
	ListSortType mSortType;

	ListAreaToolTipPtr mItemToolTip;

	FontPtr mFont;
	int mTextColor;
	SelectionColor mSelColor;

	ListItem *mSelItem;
	ListItem *mKeyItem;
	ListItem *mMouseItem;
	int mSelItemItr;

public:
	typedef int(*SortFunc)(ListArea *theListArea, const ListItem *l1, const ListItem *l2);
	static int DefaultSortFunc(ListArea *theListArea, const ListItem *l1, const ListItem *l2) { return l1->Compare(l2, theListArea); }
	static int NumberSortFunc(ListArea *theListArea, const ListItem *l1, const ListItem *l2);

protected:
	struct LineSearchPred
	{
		bool operator()(const ListItem *l1, const ListItem *l2) { return l1->my < l2->my; }
	};
	ListItemPtr mSearchLine;
	SortFunc mSortFunc;

public:
	struct LineSortPred
	{
		ListArea *mListArea;
		SortFunc mSortFunc;
		bool mAscending;
		LineSortPred(ListArea *theArea, SortFunc theFunc, ListSortType theType) : mListArea(theArea), mSortFunc(theFunc), mAscending(theType==ListSortType_Ascending) { }
		bool operator()(const ListItem *l1, const ListItem *l2) 
		{ 
			if(mAscending)
				return mSortFunc(mListArea, l1, l2)<0;
			else
				return mSortFunc(mListArea, l1, l2)>0;
		}
	};



protected:
	LineVector::iterator FindLineAt(int theYPos);
	ListItem* FindItemAt(int theYPos);
	SortFunc GetSortFunc();

	bool SetSelItem(ListItem *theItem, bool add = false, bool fireEventOnChange = true);
	void CheckMouseSel(ListItem *theItem, int x, int y, bool fromMouseDown = false);
	void FindTopLine();
	virtual void AdjustPosition(bool becauseOfScrollbar = false);

	void EnforceInvariants();
	int FindInsertPos(ListItem *theItem, int thePos);
	void DeleteItemPrv(int thePos);
	virtual void NotifyMouseItemGone();
	bool IsItemVisible(ListItem *theItem);
	ListItem* FindSelectableItem(int theStartPos, bool up, bool tryOpposite = true);

	void ItemMouseAt(int &x, int &y);
	virtual void ListSelChangedHook();
	int ClearSelectionPrv(bool fireEventOnChange = false);
	void FindNextSelection();


public:
	const SelectionColor& GetSelColor() { return mSelColor; }
	int GetPaintColumnWidth() { return mPaintColumnWidth; }
	Font* GetFont() { return mFont; }
	int GetTextColor();

public:
	virtual void SizeChanged();
	virtual void ScrollPaint(Graphics &g);

	virtual void MouseEnter(int x, int y);
	virtual void MouseMove(int x, int y);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();
	virtual void MouseDrag(int x, int y);

	virtual bool KeyDown(int theKey);

public:
	ListArea();

	// Update
	ListItem* InsertString(const GUIString &theStr, int thePos = -1);
	ListItem* InsertItem(ListItem *theItem, int thePos = -1);
	void UpdateItem(ListItem *theItem);
	void DeleteItem(int thePos);
	void DeleteItem(ListItem *theItem);
	void DeleteRange(int theStartPos, int theEndPos); 
	void Clear(bool fireEventOnChange = true);
	virtual bool Sort();

	// Use these when doing multiple updates in a row
	void BeginMultiChange();
	void EndMultiChange();

	// List Info
	int GetNumItems() { return mLineVector.size(); }
	int GetItemPos(ListItem *theItem);
	ListItem* GetItem(int thePos);
	ListItem* GetSelItem() { return mSelItem; }
	int GetItemPosAtYPos(int theYPos);
	ListItem* GetItemAtYPos(int theYPos);
	int GetSelItemPos();

	// Properties
	void SetListFlags(int theFlags, bool on);
	void SetFont(Font *theFont) { mFont = theFont; }
	void SetTextColor(int theColor) { mTextColor = theColor; }
	void SetSelectionColor(const SelectionColor &theSelColor) { mSelColor = theSelColor; }
	void SetSortType(ListSortType theType);
	void SetSortFunc(SortFunc theFunc) { mSortFunc = theFunc; }
	void ClearSelection(bool fireEventOnChange = true);

	bool ListFlagSet(int theFlags) { return mListFlags&theFlags?true:false; } 

	// Selection
	void AddSelItem(int theIndex, bool fireEventOnChange = true);
	void SetSelItem(int theIndex, bool fireEventOnChange = true);
	void SelectRange(int theStartIndex, int theEndIndex);
	void RewindSelections();
	bool HasMoreSelections();
	ListItem* GetNextSelection();

	// Item visiblity 
	void EnsureVisible(ListItem *theItem);


};
typedef WONAPI::SmartPtr<ListArea> ListAreaPtr;


}; // namespace WONAPI

#endif