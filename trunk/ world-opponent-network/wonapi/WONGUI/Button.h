#ifndef __WON_BUTTON_H__
#define __WON_BUTTON_H__

#include "Component.h"
#include "Sound.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ButtonFlags
{
	ButtonFlag_InvalidateOnMouseOver	= 0x0001,
	ButtonFlag_RepeatFire				= 0x0002,
	ButtonFlag_Checked					= 0x0004,
	ButtonFlag_Radio					= 0x0008,
	ButtonFlag_MouseOver				= 0x0010,
	ButtonFlag_ButtonDown				= 0x0020,
	ButtonFlag_ActivateDown				= 0x0040,
	ButtonFlag_EndDialog				= 0x0100,
	ButtonFlag_NoActivate				= 0x0200,
	ButtonFlag_WantDefault				= 0x0400
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Button;
typedef SmartPtr<Button> ButtonPtr;

class Button : public Component
{
protected:
	int mButtonFlags;
	bool mKeyDown;
	int mDelayAcc;

	SoundPtr mClickSound;

	enum MouseDownState { MouseDownState_Up, MouseDownState_DownNotOver, MouseDownState_Down };
	unsigned char mMouseDownState;

	virtual ~Button();
	void ButtonDown();
	void ButtonUp(bool activate);

	Button* GetNextRadio(bool forward);
	Button* GetEndRadio(bool forward);

	static ButtonPtr mDefaultButton;
	static ButtonPtr mDialogDefaultButton;

	virtual bool ActivateHook() { return true; } // return false if you don't want to fire an event

public:
	virtual void MouseEnter(int x, int y);
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseDrag(int x, int y);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();

	virtual void MouseHover(DWORD theElapsed);

	virtual void GotFocus();
	virtual void LostFocus();
	virtual bool KeyDown(int theKey);
	virtual bool KeyUp(int theKey);

	virtual bool TimerEvent(int theDelta);
	virtual void AddedToParent();

	void Activate();

	bool IsMouseButtonDown() { return mMouseDownState!=MouseDownState_Up; }
	void CopyAttributes(Button *theCopyFrom);

	static void SetDialogDefaultButton(Button *theDefaultButton);
	static void UnSetDialogDefaultButton(Button *theDefaultButton);
	static void SetDefaultButton(Button *theDefaultButton);
	static void UnSetDefaultButton(Button *theDefaultButton);

	virtual void SetTextHook(const GUIString &/*theText*/) { }

public:
	Button();

	virtual void SetText(const GUIString &theText, bool invalidate = true);
	virtual const GUIString& GetText() { return GUIString::EMPTY_STR; }

	bool ButtonFlagSet(int theFlags) { return mButtonFlags&theFlags?true:false; }
	void SetButtonFlags(int theFlags, bool on);
	
	bool WantInvalidateOnMouseOver() { return ButtonFlagSet(ButtonFlag_InvalidateOnMouseOver); }
	bool IsRepeatFire() { return ButtonFlagSet(ButtonFlag_RepeatFire); }
	bool IsChecked() { return ButtonFlagSet(ButtonFlag_Checked); }
	bool IsRadio() { return ButtonFlagSet(ButtonFlag_Radio); }
	bool IsMouseOver(); 
	bool IsButtonDown() { return ButtonFlagSet(ButtonFlag_ButtonDown); }
	bool IsDefaultButton();

	void SetCheck(bool checked, bool sendEvent = false);
	void ActivateCheck(bool checked); // activates the button if its checked state is !checked

	void SetClickSound(Sound *theSound);
};

}; // namespace WONAPI

#endif