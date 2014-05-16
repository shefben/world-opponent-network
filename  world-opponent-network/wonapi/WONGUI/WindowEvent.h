#ifndef __WON_WINDOWEVENT_H__
#define __WON_WINDOWEVENT_H__

#include "WONCommon/SmartPtr.h"
#include "EventTypes.h"
#include "GUITypes.h"

namespace WONAPI
{

class Window;
class Component;
typedef SmartPtr<Component> ComponentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WindowEventBase : public RefCount
{
public:
	WindowEventType mType;
	Window *mWindow;

public:
	WindowEventBase(WindowEventType theType = WindowEventType_None) : mWindow(NULL), mType(theType) { }
	virtual void Deliver() = 0;
};
typedef WONAPI::SmartPtr<WindowEventBase> WindowEventBasePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class InputEvent : public WindowEventBase
{
public:
	InputEvent(WindowEventType theType) : WindowEventBase(theType) { }
	virtual void Deliver();
};
typedef SmartPtr<InputEvent> InputEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MouseEvent : public InputEvent
{
public:
	int mx, my, mKeyMod, mButtonOrAmount;
	MouseEvent(WindowEventType theType, int x, int y, int keyMod, int buttonOrAmount = 0) : InputEvent(theType), mx(x), my(y), mKeyMod(keyMod), mButtonOrAmount(buttonOrAmount) { }
	virtual void Deliver();
};
typedef SmartPtr<MouseEvent> MouseEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SizeEvent : public InputEvent
{
public:
	int mx, my, mWidth, mHeight;
	SizeEvent(int x, int y, int theWidth, int theHeight) : InputEvent(InputEvent_SizeChange), mx(x), my(y), mWidth(theWidth), mHeight(theHeight) { }
};
typedef SmartPtr<SizeEvent> SizeEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class KeyEvent : public InputEvent
{
public:
	int mKey, mKeyMod;
	KeyEvent(WindowEventType theType, int theKey, int theKeyMod) : InputEvent(theType), mKey(theKey), mKeyMod(theKeyMod) { }
	virtual void Deliver();
};
typedef SmartPtr<KeyEvent> KeyEventPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentEvent : public WindowEventBase
{
protected:
	SmartPtr<Component> mComponentRef;

public:
	int mType;
	bool mBroadcast;
	Component *mComponent;

public:
	ComponentEvent(Component *theComponent, int theType);
	virtual void Deliver();

	void SetComponent(Component *theComponent);
	int GetControlId();
};
typedef SmartPtr<ComponentEvent> ComponentEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UserEvent : public WindowEventBase
{
protected:
	typedef void(*Callback)(void* theUserData);

	Callback	mCallback;
	void*		mUserData;

public:
	UserEvent(Callback theCallback, void* theUserData = NULL) : mCallback(theCallback), mUserData(theUserData) { } 
	virtual void Deliver() { mCallback(mUserData); }
};
typedef SmartPtr<UserEvent> UserEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DoPopupEvent : public WindowEventBase
{
protected:
	PopupParams mParams;

public:
	DoPopupEvent(const PopupParams &theParams) : mParams(theParams) { }
	virtual void Deliver();
};
typedef SmartPtr<DoPopupEvent> DoPopupEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class EndDialogEvent : public WindowEventBase
{
protected:
	WindowPtr mWindowRef;
	ComponentPtr mComponent;
	int mResult;

	virtual ~EndDialogEvent();

public:
	EndDialogEvent(int theResult, Window *theWindow, Component *theComponent = NULL);

	virtual void Deliver();
};
typedef SmartPtr<DoPopupEvent> DoPopupEventPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

}; // namespace WONAPI

#endif