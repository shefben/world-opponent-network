#ifndef __WON_KEYBOARDLISTENER_H__
#define __WON_KEYBOARDLISTENER_H__

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class KeyboardListenerBase : public WONAPI::RefCount
{
public:
	virtual void KeyEvent(int theKey, int theEventType) = 0; // 0 = KeyDown, 1 = KeyUp, 2 = KeyChar
};
typedef WONAPI::SmartPtr<KeyboardListenerBase> KeyboardListenerBasePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class KeyboardListener : public KeyboardListenerBase
{
protected:
	typedef void(*Callback)(int theKey, int theEventType);
	Callback mCallback;

public:
	KeyboardListener(Callback theCallback) : mCallback(theCallback) {}
	virtual void KeyEvent(int theKey, int theEventType) { mCallback(theKey, theEventType); }

};
typedef WONAPI::SmartPtr<KeyboardListener> KeyboardListenerPtr;

}; // namespace WONAPI

#endif