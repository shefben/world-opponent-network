#ifndef __WON_MENUBAR_H__
#define __WON_MENUBAR_H__

#include "Container.h"
#include "Button.h"
#include "ListBox.h"

#include <vector>

namespace WONAPI
{
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PopupMenu;
class MenuItem;

typedef SmartPtr<PopupMenu> PopupMenuPtr;

class PopupMenu : public ListArea
{
protected:
	PopupMenuPtr mCurSubMenu;
	ListItemPtr mCurSubMenuItem;
	int mMouseUpCount;
	int mTimerAcc;
	int mLastX, mLastY;
	bool mKeyboardMode;
	void ListSelChangedHook();

public:
	virtual void MouseMove(int x, int y);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual bool TimerEvent(int theDelta);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	bool TryKeyDown(int theKey);
	void EndSubMenu();
	void CheckSubMenu();

public:
	PopupMenu();
	MenuItem* AddItem(const GUIString &theText, int theId = 0);
	MenuItem* AddItem(const GUIString &theText, ComponentListener *theListener, int theId = 0);
	MenuItem* AddItem(const GUIString &theText, NativeImage *theIcon, ComponentListener *theListener = NULL, int theId = 0);
	PopupMenu* AddSubMenu(const GUIString &theText, PopupMenu *theSubMenu = NULL);
	void AddSeperator();

	void Reset();
	bool IsShowing();
	void DoPopup(Window *theOwnerWindow, Component *theOwnerComponent, int x = -1, int y = -1);
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MenuItem : public TextListItem
{
public:
	PopupMenuPtr mSubMenu;
	int mMenuId;
	bool mDisabled;
	ComponentListenerPtr mListener;

	virtual void CalcDimensions(ListArea *theListArea);
	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual bool Contains(int x, int y) { return y>=my && y<my+mHeight; }

public:
	MenuItem(const GUIString &theText, ComponentListener *theListener = NULL);

	bool HasSubMenu() { return mSubMenu.get()!=NULL; }
	bool IsDisabled() { return mDisabled; }
	bool IsSelectable() { return !HasSubMenu() && !IsDisabled(); }
	int GetMenuId()		{ return mMenuId;}
};
typedef SmartPtr<MenuItem> MenuItemPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MenuItemSelectedEvent : public ComponentEvent
{
public:
	MenuItemPtr mItem;

	virtual void Deliver();

public:
	MenuItemSelectedEvent(Component *theMenu, MenuItem *theItem) : 
	  ComponentEvent(theMenu, ComponentEvent_MenuItemSelected), mItem(theItem) { }
};
typedef SmartPtr<MenuItemSelectedEvent> MenuItemSelectedEventPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum MenuBarState 
{
	MenuBarState_None,
	MenuBarState_Up,
	MenuBarState_Down
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MenuBarItem : public Component
{
protected:
	PopupMenuPtr mPopup;
	FontPtr mFont;
	GUIString mText;

	MenuBarState mState;

public:
	virtual void Paint(Graphics &g);

public:
	MenuBarItem();

	void SetMenuBarState(MenuBarState theState);
	void SetText(const GUIString &theText);
	void SetPopup(PopupMenu *thePopup);
	
	PopupMenu* GetPopup() { return mPopup; }
};
typedef SmartPtr<MenuBarItem> MenuBarItemPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MenuBar : public Container
{
protected:
	typedef std::vector<MenuBarItem*> ItemVector;
	ItemVector mItemVector;

	MenuBarState mState;
	MenuBarItem* mCurItem;
	int mLastX, mLastY;
	Background mBackground;

	void CheckItem(Component *theCheck);

public:
	virtual void MouseDrag(int x, int y);
	virtual void MouseMove(int x, int y);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();
	virtual bool KeyDown(int theKey);

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void Paint(Graphics &g);

public:
	MenuBar();
	void AddItem(const GUIString &theText, PopupMenu *thePopup);
	void SetBackground(const Background &theBackground) { mBackground = theBackground; ApplyBackground(mBackground); }
	void AddSeperator(Component *theComponent = NULL);


};
typedef SmartPtr<MenuBar> MenuBarPtr;

}; //namespace WONAPI;
#endif