#ifndef __WON_COMPONENT_H__
#define __WON_COMPONENT_H__

#pragma warning(disable:4786)

#include "WONCommon/SmartPtr.h"
#include "Graphics.h"
#include "Rectangle.h"
#include "Keyboard.h"
#include "EventTypes.h"
#include "ComponentListener.h"
#include "WindowEvent.h"
#include "Rectangle.h"

#include <list>
#include <set>

namespace WONAPI
{

class Component;
class Container;
class RootContainer;
class Window;
class BackgroundGrabber;
class Cursor;
class ColorScheme;
class Background;

typedef WONAPI::SmartPtr<Component> ComponentPtr;
typedef WONAPI::SmartPtr<Cursor> CursorPtr;
typedef std::list<ComponentPtr> ChildList;
typedef std::set<ComponentPtr,ComponentPtr::Comp> ChildSet;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ComponentFlags
{
	ComponentFlag_Invisible				= 0x000001,
	ComponentFlag_WantFocus				= 0x000008,
	ComponentFlag_HasFocus				= 0x000010,
	ComponentFlag_ParentDrag			= 0x000020,
	ComponentFlag_NoParentCursor		= 0x000040, // parent shouldn't change cursor if his MouseChild has this flag set. 
	ComponentFlag_Disabled				= 0x000080,
	ComponentFlag_GrabBG				= 0x000100,
	ComponentFlag_Translucent			= 0x000200,
	ComponentFlag_NoInput				= 0x000800,
	ComponentFlag_AlwaysOnTop			= 0x001000,
	ComponentFlag_Clip					= 0x002000,
	ComponentFlag_WantTabFocus			= 0x004000,
	ComponentFlag_AllowModalInput		= 0x008000,
	ComponentFlag_IsPopup				= 0x010000,
	ComponentFlag_DebugBG				= 0x020000,
	ComponentFlag_IsVisibleToRoot		= 0x040000
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ControlIdSet
{
protected:
	typedef std::set<int> IdSet;
	IdSet mIdSet;

public:
	ControlIdSet() { }
	ControlIdSet(int theId) { AddId(theId); }

	void Clear() { mIdSet.clear(); }

	void AddId(int theId) { mIdSet.insert(theId); }
	void AddIds(int id1, int id2) { mIdSet.insert(id1); mIdSet.insert(id2); }
	void AddIds(int id1, int id2, int id3) { mIdSet.insert(id1); mIdSet.insert(id2); mIdSet.insert(id3); }
	void AddIds(int id1, int id2, int id3, int id4) { mIdSet.insert(id1); mIdSet.insert(id2); mIdSet.insert(id3); mIdSet.insert(id4); }
	void AddIds(int id1, int id2, int id3, int id4, int id5) { mIdSet.insert(id1); mIdSet.insert(id2); mIdSet.insert(id3); mIdSet.insert(id4); mIdSet.insert(id5); }
	void AddIds(int id1, int id2, int id3, int id4, int id5, int id6) { mIdSet.insert(id1); mIdSet.insert(id2); mIdSet.insert(id3); mIdSet.insert(id4); mIdSet.insert(id5); mIdSet.insert(id6);}
	bool HasId(int theId) const { return mIdSet.find(theId)!=mIdSet.end(); }
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Component : public WONAPI::RefCount
{
protected:
	virtual ~Component();

	Container *mParent;
	ChildList::iterator mChildListItr;

	WONRectangle mBounds;
	int mComponentFlags;
	int mControlId;
	unsigned char mFocusId;

	enum { InvalidState_Valid = 0, InvalidState_PartiallyInvalid = 1, InvalidState_SelfInvalid = 2, InvalidState_FullyInvalid = 3};
	char mInvalidState;

	enum { TimerFlag_DontWant = 0, TimerFlag_ChildWants = 1, TimerFlag_IWant = 2};
	char mTimerFlags;

	friend class ComponentExtra;
	ComponentExtra *mExtraStuff;

	bool CheckGrabBG();
	void AllocateExtraStuff();

public:
	virtual void PosChanged();
	virtual void SizeChanged();
	virtual void GetDesiredSize(int &width, int &height);
	
	virtual bool NeedPrePaint();
	virtual void PrePaint(Graphics &g);
	virtual void Paint(Graphics &g);
	virtual void InitComponent(ComponentInit &/*theInit*/) { }

	virtual bool WantInput(int x, int y);
	virtual void MouseEnter(int x, int y);
	virtual void MouseExit();

	virtual void MouseDown(int /*x*/, int /*y*/, MouseButton /*theButton*/) { }
	virtual void MouseUp(int /*x*/, int /*y*/, MouseButton /*theButton*/) { }

	virtual void MouseMove(int /*x*/, int /*y*/) { }
	virtual void MouseDrag(int /*x*/, int /*y*/) { }

	virtual bool MouseWheel(int /*theAmount*/) { return false; }
	virtual void MouseHover(DWORD theElapsed);

	virtual bool TimerEvent(int /*theDelta*/) { return true; }

	virtual void GotFocus();
	virtual void LostFocus();
	virtual bool KeyChar(int /*theKey*/) { return false; }
	virtual bool KeyDown(int /*theKey*/) { return false; }
	virtual bool KeyUp(int /*theKey*/) { return false; }
	virtual bool AllowWindowClose() { return true; }

	virtual void InvalidateRect(WONRectangle &theRect);
	virtual void Invalidate();
	void InvalidateFully();
	virtual void InvalidateDown();
	virtual void AddedToParent();
	virtual void AddedToRoot();
	virtual void EnableHook(bool /*isEnabled*/) { }

	virtual void SetCursor(Cursor *theCursor);
	void SetDefaultCursor(Cursor *theCursor);

	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	ColorScheme* GetOwnColorScheme();
	ColorScheme* GetColorScheme();
	void SetColorScheme(ColorScheme* theColorScheme);
	void SetFocusId(unsigned char theId) { mFocusId = theId; }
	unsigned char GetFocusId() { return mFocusId; }

	void SubscribeToBroadcast(bool subscribe);
	void BroadcastEvent(int theEvent, bool sendNow = false);
	void BroadcastEvent(ComponentEvent *theEvent, bool sendNow = false);
	virtual void BroadcastEventDownPrv(ComponentEvent *theEvent);
	void BroadcastEventDown(ComponentEvent *theEvent);
	void FireEvent(int theEvent, bool sendNow = false);
	void FireEvent(ComponentEvent *theEvent, bool sendNow = false);
	void DeliverComponentEvent(ComponentEvent *theEvent);
	BackgroundGrabber* GetGrabBG();
	void ApplyBackground(const Background &theBackground);

	virtual void NotifyRootVisibilityChange(bool isVisible);

public:
	Component();

	Container* GetParent() { return mParent; }
	void SetParent(Container *theParent) { mParent = theParent; }
	void SetChildListItr(const ChildList::iterator &theItr) { mChildListItr = theItr; }
	ChildList::iterator& GetChildListItr() { return mChildListItr; }

	void SetLeft(int theLeft) { SetPos(theLeft,Top()); }
	void SetTop(int theTop) { SetPos(Left(),theTop); }
	void SetWidth(int theWidth) { SetSize(theWidth,Height()); }
	void SetHeight(int theHeight) { SetSize(Width(),theHeight); }
	void SetPos(int x, int y);
	void SetSize(int width, int height);
	void SetSizeAtLeast(int width, int height);
	void SetPosSize(int x, int y, int width, int height);
	void SetDesiredSize(int theExtraWidth = 0, int theExtraHeight = 0);
	void SetDesiredWidth(int theExtraWidth = 0);
	void SetDesiredHeight(int theExtraHeight = 0);

	void SetTranslucentColor(int theColor, int theLevel = 4);

	void BringToTop();
	void EnsureOnTopOf(Component *theSibling); 

	bool Contains(int x, int y);
	bool Intersects(Component *theComponent);

	bool IsInvalid() { return mInvalidState!=InvalidState_Valid; }
	bool IsSelfInvalid() { return mInvalidState>=InvalidState_SelfInvalid; }
	bool IsFullyInvalid() { return mInvalidState==InvalidState_FullyInvalid; }

	int GetComponentFlags() { return mComponentFlags; }
	bool ComponentFlagSet(int theFlags) { return mComponentFlags&theFlags?true:false; }
	void SetComponentFlags(int theFlags, bool on);
	bool IsVisible() { return !ComponentFlagSet(ComponentFlag_Invisible); }
	bool IsVisibleUpToRoot();
	bool WantTimer() { return mTimerFlags!=0; } //ComponentFlagSet(ComponentFlag_WantTimer); }
	bool WantFocus();
	bool HasFocus() { return ComponentFlagSet(ComponentFlag_HasFocus); }
	bool Disabled() { return ComponentFlagSet(ComponentFlag_Disabled); }

	virtual bool TakeTabFocus(bool forward, bool init);
	virtual void EnableControls(const ControlIdSet &theSet, bool enable);
	virtual void Enable(bool isEnabled);
	virtual void SetVisible(bool isVisible);
	void RequestTimer(bool on);
	void RequestFocus();

	void ChildToRoot(int &x, int &y);
	void RootToChild(int &x, int &y);
	void GetMousePos(int &x, int &y);
	int GetKeyMod();
	bool CheckDoubleClick(MouseButton theButton);

	Component* GetNextSibling(bool forward = true, bool wrap = false);
	RootContainer* GetRoot();
	Window* GetWindow();
	Window* GetMyWindow();
	FontPtr GetFont(const FontDescriptor &theDescriptor);
	Font* GetFontMod(Font *theBaseFont, int theStyle, int theSize = 0);
	Font* GetDefaultFont();
	Font* GetNamedFont(const std::string &theName);
	Font* GetDefaultFontMod(int theStyle, int theSize = 0);
	Font* GetNamedFontMod(const std::string &theName, int theStyle, int theSize = 0);

	Cursor* GetDefaultCursor();

	virtual Component* GetMouseChild() { return NULL; }

	void SetListener(ComponentListener *theListener);
	void SetControlId(int theId) { mControlId = theId; }
	int  GetControlId() { return mControlId; }

	void SetToolTip(Component *theTip);
	void SetToolTipTimes(DWORD theStartTime, DWORD theEndTime);


	int Left() { return mBounds.Left(); }
	int Right() { return mBounds.Right(); }
	int Top() { return mBounds.Top(); }
	int Bottom() { return mBounds.Bottom(); }
	int Width() { return mBounds.Width(); }
	int Height() { return mBounds.Height(); }
	virtual int MinWidth() { return 0; }
	virtual int MinHeight() { return 0; }
	virtual int MaxWidth() { return 0xffff; }
	virtual int MaxHeight() { return 0xffff; }

	const WONRectangle& GetBounds() { return mBounds; }
};

}; // namespace WONAPI

#endif