#ifndef __WON_CONTAINER_H__
#define __WON_CONTAINER_H__

#include "Component.h"
#include "Cursor.h"
#include "Rectangle.h"
#include "ChildLayoutManager.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Container : public Component
{
protected:
	virtual ~Container();

	Component *mMouseChild;
	Component *mFocusChild;
	ChildList mChildList;
	ChildSet mChildTimerSet;
	ChildLayoutManager mChildLayout;
	unsigned char mNextFocusId;

	CursorPtr mCurrentCursor;

	void MouseAt();
	void MouseAt(int x, int y);

	friend class Component;
	bool mChildNeedsTimer;
	bool mMouseWheelGoesToFocusChild;
	bool mIsolateTabFocus;
	bool mHasUsedDialogKeys;
	bool mOnlyChildInput;
	bool mRemovingAllChildren;

	static bool mMouseWheelGoesToFocusChildDef;

	int mModalCount;

	ChildList::iterator FindTop();
	ChildList::iterator FindTop(ChildList::iterator theItr);

	virtual void BroadcastEventDownPrv(ComponentEvent *theEvent);

	unsigned char GetNextFocusId();
	void SetFocusToLastFocusChild();

public:	
	virtual Component* FindChildAt(int x, int y);
	virtual Component* WMFindChildAt(int x, int y) { return FindChildAt(x,y); }

	virtual void SizeChanged();
	virtual void GetDesiredSize(int &width, int &height);
	virtual void PrePaint(Graphics &g);
	virtual void Paint(Graphics &g);
	virtual bool WantInput(int x, int y);
	virtual bool TakeTabFocus(bool forward, bool init);

	virtual void MouseEnter(int x, int y);
	virtual void MouseExit();

	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);

	virtual void MouseMove(int x, int y);
	virtual void MouseDrag(int x, int y);

	virtual bool MouseWheel(int theAmount);
	virtual void MouseHover(DWORD theElapsed);

	virtual bool KeyChar(int theKey);
	virtual bool KeyDown(int theKey);
	virtual bool KeyUp(int theKey);
	virtual bool AllowWindowClose();

	virtual void GotFocus();
	virtual void LostFocus();
	virtual void ChildRequestFocus(Component *theChild);
	virtual void ChildRequestTimer(Component *theChild);
	virtual void ChildChanged(Component *theChild);

	virtual bool TimerEvent(int theDelta);

	virtual void InvalidateDown();
	virtual bool InvalidateUp(Component *theChild);
	virtual void Invalidate();
	virtual void AddedToParent();
	virtual void AddedToRoot();

	Cursor* GetCurrentCursor() { return mCurrentCursor; }

	void EnterModal();
	void LeaveModal();
	bool IsModal() { return mModalCount>0; }

	bool GetHasUsedDialogKeys() { return mHasUsedDialogKeys; }
	void SetHasUsedDialogKeys(bool theVal) { mHasUsedDialogKeys = theVal; }
	bool GetIsolateTabFocus() { return mIsolateTabFocus; }
	virtual void NotifyRootVisibilityChange(bool isVisible);


public:
	Container();
	virtual void SetCursor(Cursor *theCursor);
	void SetIsolateTabFocus(bool isolate) { mIsolateTabFocus = isolate; }
	void SetOnlyChildInput(bool onlyChildInput) { mOnlyChildInput = onlyChildInput; }
	
	void AddChild(Component *theChild);
	void AddChildEx(Component *theChild, int thePos);
	virtual void RemoveChild(Component *theChild);
	void RemoveAllChildren();
	void BringToTop(Component *theChild);
	void BringToTop() { Component::BringToTop(); }
	void EnsureOnTopOf(Component *theBottom, Component *theTop);

	virtual void EnableControls(const ControlIdSet &theSet, bool enable);

	virtual Component* GetComponentById(int theControlId);
	virtual Component* GetMouseChild() { return mMouseChild; }
	Component* GetFocusChild() { return mFocusChild; }
	Component* GetFirstChild() { return mChildList.empty()?NULL:mChildList.front(); }

	static void SetMouseWheelGoesToFocusChildDef(bool on) { mMouseWheelGoesToFocusChildDef = on; }
	void SetMouseWheelGoesToFocusChild(bool on) { mMouseWheelGoesToFocusChild = on; }
	void AddChildLayout(Component *target, int theType, Component *ref, int horzPosPad = 0, int vertPosPad = 0, int horzSizePad = 0, int vertSizePad = 0, int theId = 0);
	void AddChildLayout(ChildLayoutInstructionBase *theInstr, int theId = 0);
	void AddLayoutPoint(LayoutPoint *thePoint);
	void ClearChildLayouts();
	void EnableLayout(int theId, bool enable);
	void RecalcLayout();
};

typedef WONAPI::SmartPtr<Container> ContainerPtr;


}; // namespace WONAPI

#endif