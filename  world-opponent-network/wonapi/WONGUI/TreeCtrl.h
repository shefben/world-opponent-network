#ifndef __WON_TREECTRL_H__
#define __WON_TREECTRL_H__

#include "ListBox.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TreeItem;
class TreeArea;
typedef WONAPI::SmartPtr<TreeItem> TreeItemPtr;

class TreeItem : public ListItem
{
protected:
	TreeItem *mParent;
	int mx, mPos;
	bool mIsOpen;
	ListItemPtr mItem;
	typedef std::vector<TreeItemPtr> ItemVector;
	ItemVector mChildren;

	TreeItem* GetNextNonChild();
	int GetNextNonChildPos(TreeArea *theTreeArea);
	bool IsLastChild();

	void CloseAll();

	void RemoveChild(TreeArea *theTreeArea, TreeItem *theItem);
	void RemoveAllChildren(TreeArea *theTreeArea);
	void RemoveFromParent(TreeArea *theTreeArea);
	void InsertChildren(TreeArea *theTreeArea, int thePos);
	int GetInsertPos(TreeArea *theTreeArea, TreeItem *theItem, int thePos);
	void InsertChild(TreeArea *theTreeArea, TreeItem *theItem, int thePos = -1);
	void SetOpen(TreeArea *theTreeArea, bool isOpen, bool closeAll = false, bool sendEvent = true);
	void ToggleOpen(TreeArea *theTreeArea, bool closeAll = false, bool sendEvent = true);

	virtual void SetPos(int x, int y);
	ListItem* GetMouseItem(ListArea *theListArea, int &x, int &y);

	void Sort(TreeArea *theTreeArea, bool recursive);

	friend class TreeArea;

public:
	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual void SetViewport(ListArea *theListArea, Container *theViewport);
	virtual void CalcDimensions(ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
	virtual bool CheckMouseSel(ListArea *theListArea, int x, int y);

	virtual void MouseEnter(ListArea *theListArea, int x, int y); 
	virtual void MouseExit(ListArea *theListArea);

	virtual void MouseMove(ListArea *theListArea, int x, int y);
	virtual void MouseDrag(ListArea *theListArea, int x, int y);

	virtual void MouseDown(ListArea *theListArea, int x, int y, MouseButton theButton);
	virtual void MouseUp(ListArea *theListArea, int x, int y, MouseButton theButton);

public:
	TreeItem();

	bool IsOpen() { return mIsOpen; }
	TreeItem* GetParent() { return mParent; }
	TreeItem* GetNextSibling();

	int GetNumChildren() { return mChildren.size(); }
	int GetPos() { return mPos; }
	TreeItem* GetChild(int thePos);

	ListItem* GetItem() { return mItem; }
	bool IsInItem(int x, int y);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TreeItemOpenedEvent : public ComponentEvent
{
public:
	TreeItemPtr mItem;

	TreeItemOpenedEvent(Component *theComponent, TreeItem *theItem) : 
		ComponentEvent(theComponent, ComponentEvent_TreeItemOpened), mItem(theItem) { }
};
typedef SmartPtr<TreeItemOpenedEvent> TreeItemOpenedEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum TreeFlags
{
	TreeFlag_DrawLines			= 0x0001,
	TreeFlag_DrawButtons		= 0x0002,
	TreeFlag_CloseAll			= 0x0004,
	TreeFlag_DoubleClickOpen	= 0x0008

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TreeArea : public ListArea
{
protected:
	friend class TreeItem;
	TreeItemPtr mRoot;
	int mTreeFlags;
	ListItem *mMouseColItem;
	ListSortType mTreeSortType;



	// Protect base class methods so user doesn't accidently call them
	void DeleteItem(int thePos) { ListArea::DeleteItem(thePos); }
	void DeleteRange(int theStartPos, int theEndPos) { ListArea::DeleteRange(theStartPos, theEndPos); } 

	virtual void NotifyMouseItemGone();

public:
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	TreeArea();

	// Update
	TreeItem* InsertItem(TreeItem *theParent, ListItem *theItem, int thePos = -1);
	TreeItem* InsertString(TreeItem *theParent, const GUIString &theStr, Image *theIcon = NULL, int thePos = -1);
	void DeleteItem(TreeItem *theItem);
	void DeleteAllChildren(TreeItem *theParent);
	void Clear(bool fireEventOnChange = true);

	// Open/Close 
	void OpenItem(TreeItem *theItem, bool sendEvent = true);
	void CloseItem(TreeItem *theItem, bool closeAll = false, bool sendEvent = true);
	void ToggleItem(TreeItem *theItem, bool closeAll = false, bool sendEvent = true);

	// Tree Info
	TreeItem* GetItem(int thePos) { return (TreeItem*)ListArea::GetItem(thePos); }
	TreeItem* GetSelItem() { return (TreeItem*)ListArea::GetSelItem(); }
	TreeItem* GetNextSelection() { return (TreeItem*)ListArea::GetNextSelection(); }

	// Properties
	void SetTreeFlags(int theFlags, bool on);
	bool TreeFlagSet(int theFlags) { return mTreeFlags&theFlags?true:false; }

	void SetSortType(ListSortType theType);
	void Sort(TreeItem *theItem, bool recursive = false);
	bool Sort();

	TreeItem* GetRootItem() { return mRoot; }
};
typedef WONAPI::SmartPtr<TreeArea> TreeAreaPtr;

}; // namespace WONAPI

#endif
