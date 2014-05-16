#include "Button.h"
#include "Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ButtonPtr Button::mDefaultButton;
ButtonPtr Button::mDialogDefaultButton;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Button::Button()
{
	mButtonFlags = 0;
	mKeyDown = false;
	mMouseDownState = MouseDownState_Up;
	SetComponentFlags(ComponentFlag_WantTabFocus,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Button::~Button()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Button::IsMouseOver()
{
	return ButtonFlagSet(ButtonFlag_MouseOver); 
//	if(GetParent()==NULL)
//		return false;
//	else
//		return GetParent()->GetMouseChild()==this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::SetText(const GUIString &theText, bool invalidate)
{
	SetTextHook(theText);
	if(invalidate)
		Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Button::IsDefaultButton()
{
	return mDefaultButton.get()==this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::SetDialogDefaultButton(Button *theDefaultButton)
{
	if(theDefaultButton==mDialogDefaultButton)
		return;

	mDialogDefaultButton = theDefaultButton;
	SetDefaultButton(theDefaultButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::UnSetDialogDefaultButton(Button *theDefaultButton)
{
	if(theDefaultButton!=mDialogDefaultButton)
		return;

	mDialogDefaultButton = NULL;
	UnSetDefaultButton(theDefaultButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::SetDefaultButton(Button *theDefaultButton)
{
	if(theDefaultButton==mDefaultButton)
		return;

	if(mDefaultButton.get()!=NULL)
		mDefaultButton->Invalidate();

	mDefaultButton = theDefaultButton;
	if(theDefaultButton!=NULL)
		theDefaultButton->Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::UnSetDefaultButton(Button *theDefaultButton)
{
	SetDefaultButton(mDialogDefaultButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::CopyAttributes(Button *theCopyFrom)
{
	mButtonFlags = theCopyFrom->mButtonFlags;
	mClickSound = theCopyFrom->mClickSound;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::SetButtonFlags(int theFlags, bool on)
{
	if(on)
		mButtonFlags |= theFlags;
	else
		mButtonFlags &= ~theFlags;

	if(IsRadio())
		SetComponentFlags(ComponentFlag_WantFocus, IsChecked());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::AddedToParent()
{
	Component::AddedToParent();
	SetButtonFlags(ButtonFlag_ButtonDown | ButtonFlag_MouseOver,false);

	// Make sure all of the focus issues with the radio buttons are handled.
	// A checked radio wants the focus, and an unchecked radio doesn't want
	// the focus.  However, if no radio in the group is checked then one of
	// the radios should still want the focus.
	if(IsRadio())
	{
		SetCheck(IsChecked());
		bool aRadioWantsFocus = false;
		if(!WantFocus())
		{
			Button *aRadio = GetEndRadio(false);				
			while(aRadio!=NULL)
			{
				if(aRadio->WantFocus())
				{
					aRadioWantsFocus = true;
					break;
				}
				
				aRadio = aRadio->GetNextRadio(true);
			}
			
			if(!aRadioWantsFocus)
				SetComponentFlags(ComponentFlag_WantFocus,true);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::Activate()
{
	if(Disabled() || ButtonFlagSet(ButtonFlag_NoActivate))
		return;

	if(IsRadio())
		SetCheck(true);
	else 
		SetCheck(!IsChecked());

	if(ActivateHook())
		FireEvent(ComponentEvent_ButtonPressed); // button pressed event
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Button* Button::GetNextRadio(bool forward)
{
	Component *aComponent = GetNextSibling(forward,false);
	Button *aButton = dynamic_cast<Button*>(aComponent);
	if(aButton==NULL || !aButton->IsRadio())
		return NULL;

	return aButton;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Button* Button::GetEndRadio(bool forward)
{
	Button *curRadio = this;
	Button *nextRadio = NULL;
	while(true)
	{
		nextRadio = curRadio->GetNextRadio(forward);
		if(nextRadio==NULL)
			return curRadio;
		
		curRadio = nextRadio;
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::ActivateCheck(bool checked)
{
	if(IsChecked()!=checked)
		Activate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::SetCheck(bool checked, bool sendEvent)
{
	if(IsRadio())
	{
		SetComponentFlags(ComponentFlag_WantFocus,checked);
		if(checked)
		{
			// Make sure all other radio buttons in the group are not checked.
			Button *aRadio = GetEndRadio(false);				
			while(aRadio!=NULL)
			{
				if(aRadio!=this)
				{
					aRadio->SetComponentFlags(ComponentFlag_WantFocus,false);
					if(aRadio->IsChecked())
					{
						aRadio->SetButtonFlags(ButtonFlag_Checked, false);
						aRadio->Invalidate();
					}
				}
				
				aRadio = aRadio->GetNextRadio(true);
			}

			RequestFocus();
		}
	}

	if(sendEvent)
		FireEvent(ComponentEvent_ButtonPressed); // button pressed event
	
	if(IsChecked()==checked)
		return;

	SetButtonFlags(ButtonFlag_Checked, checked);
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::MouseEnter(int x, int y)
{
	Component::MouseEnter(x,y);

	SetButtonFlags(ButtonFlag_MouseOver, true);
	if(WantInvalidateOnMouseOver())
		Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::MouseDown(int x, int y, MouseButton theButton)
{
	Component::MouseDown(x,y,theButton);

	if(theButton==MouseButton_Left)
	{
		mMouseDownState = MouseDownState_Down;
//		Container *aParent = GetParent();
//		if(aParent!=NULL)
//			aParent->SetHasUsedDialogKeys(false);

		ButtonDown();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::MouseUp(int x, int y, MouseButton theButton)
{
	Component::MouseUp(x,y,theButton);
	if(theButton==MouseButton_Left)
	{
		mMouseDownState = MouseDownState_Up;
		ButtonUp(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::MouseExit()
{
	Component::MouseExit();
	mMouseDownState = MouseDownState_Up;
	ButtonUp(false);

	if(IsMouseOver() && WantInvalidateOnMouseOver())
		Invalidate();

	SetButtonFlags(ButtonFlag_MouseOver, false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::ButtonDown()
{
	if(IsButtonDown())
		return;

	if(mClickSound.get()!=NULL)
		mClickSound->Play();
	
	SetButtonFlags(ButtonFlag_ButtonDown,true);
	Invalidate();

	if(IsRepeatFire())
	{
		mDelayAcc = 0;
		RequestTimer(true);
		Activate();
	}

	if(ButtonFlagSet(ButtonFlag_ActivateDown))
		Activate();

	RequestFocus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::ButtonUp(bool activate)
{
	if(!IsButtonDown() || mMouseDownState==MouseDownState_Down || mKeyDown)
		return;

	if(mMouseDownState==MouseDownState_DownNotOver)
		activate = false;

	SetButtonFlags(ButtonFlag_ButtonDown, false);
	Invalidate();

	if(IsRepeatFire())
		RequestTimer(false);
	else if(activate && !(ButtonFlagSet(ButtonFlag_ActivateDown)))
		Activate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::MouseDrag(int x, int y)
{
	if(mMouseDownState==MouseDownState_Up)
		return;

	if(x<0 || y<0 || x>=Width() || y>=Height())
	{
		mMouseDownState = MouseDownState_DownNotOver;
		ButtonUp(false);
	}
	else
	{
		mMouseDownState = MouseDownState_Down;
		if(!IsButtonDown())
		{
			ButtonDown();
			mDelayAcc = 500;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::GotFocus()
{
	Component::GotFocus();
	if(ButtonFlagSet(ButtonFlag_WantDefault))
		SetDefaultButton(this);

	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::LostFocus()
{
	Component::LostFocus();
	if(mKeyDown)
	{
		mKeyDown = false;
		ButtonUp(false);
	}
	UnSetDefaultButton(this);

	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Button::KeyDown(int theKey)
{
	Component::KeyDown(theKey);

	if(theKey==KEYCODE_RETURN && IsDefaultButton())
	{
		Activate();
		return true;
	}
	else if(theKey==KEYCODE_SPACE && !mKeyDown)
	{
		mKeyDown = true;
		Container *aParent = GetParent();
		if(aParent!=NULL)
			aParent->SetHasUsedDialogKeys(true);

		ButtonDown();	
		return true;
	}
	else if(IsRadio())
	{
		bool forward;
		if(theKey==KEYCODE_UP || theKey==KEYCODE_LEFT)
			forward = false;
		else if(theKey==KEYCODE_DOWN || theKey==KEYCODE_RIGHT)
			forward = true;
		else
			return false;

		Container *aParent = GetParent();
		if(aParent!=NULL)
			aParent->SetHasUsedDialogKeys(true);

		Button *aRadio = GetNextRadio(forward);
//		if(aRadio==NULL)
//			aRadio = GetEndRadio(!forward);

		if(aRadio!=NULL)
		{
			aRadio->Activate();
			aRadio->RequestFocus();
		}

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Button::KeyUp(int theKey)
{
	Component::KeyUp(theKey);
	if(theKey==KEYCODE_SPACE)
	{
		mKeyDown = false;
		ButtonUp(true);
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Button::TimerEvent(int theDelta)
{
	mDelayAcc+=theDelta;
	if(mDelayAcc>500)
	{
		mDelayAcc-=100;
		Activate();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::MouseHover(DWORD theElapsed)
{
	Component::MouseHover(theElapsed);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Button::SetClickSound(Sound *theSound)
{
	mClickSound = theSound;
}
