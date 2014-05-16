#include "Dialog.h"
#include "Window.h"
#include "GUICompat.h"

using namespace WONAPI;
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Dialog::Dialog()
{
	mDragX = mDragY = 0;
	SetComponentFlags(ComponentFlag_WantFocus | ComponentFlag_HasFocus | ComponentFlag_GrabBG,true);
	mAllowDrag = true;
	mKeepInRoot = true;
	
	mEndOnEscape = false;
	mEscapeId  = -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::SetEndOnEscape(int theEscapeId)
{
	mEndOnEscape = true;
	mEscapeId = theEscapeId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::SetDontEndOnEscape()
{
	mEndOnEscape = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::SetDefaultButton(Button *theButton)
{
	mDefaultButton = theButton;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::GotFocus()
{
	Button::SetDialogDefaultButton(mDefaultButton);
	Container::GotFocus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::LostFocus()
{
	Button::UnSetDialogDefaultButton(mDefaultButton);
	Container::LostFocus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::SizeChanged()
{
	mDragging = false;
	Container::SizeChanged();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::MouseDown(int x, int y, MouseButton theButton)
{
	Container::MouseDown(x,y,theButton);
	if(theButton!=MouseButton_Left)
		return;

	RequestFocus();

	if(mAllowDrag)
	{
		if(mMouseChild!=NULL && mMouseChild->ComponentFlagSet(ComponentFlag_ParentDrag))
		{
			if(GetNextSibling()!=NULL)
				mParent->BringToTop(this);

			mDragging = true;
			mDragX = x;
			mDragY = y;
		}
	}
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::MouseUp(int x, int y, MouseButton theButton)
{
	Container::MouseUp(x,y,theButton);

	if(theButton==MouseButton_Left)
		mDragging = false;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::MouseExit()
{
	Container::MouseExit();
	mDragging = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::MouseDrag(int x, int y)
{
	if(!mDragging)
	{
		Container::MouseDrag(x,y);
		return;
	}

	Container *aParent = GetParent();
	int aNewX = Left() + (x - mDragX);
	int aNewY = Top() + (y - mDragY);
	if(mKeepInRoot)
	{
		if(aNewX + Width() > aParent->Width())
			aNewX = aParent->Width() - Width();
		if(aNewX<0)
			aNewX = 0;
		if(aNewY + Height() > aParent->Height())
			aNewY = aParent->Height() - Height();
		if(aNewY<0)
			aNewY = 0;
	}

	if(aNewX==Left() && aNewY==Top())
		return;

	WONRectangle r(0,0,Width(),Height());
	mParent->InvalidateUp(this);
	InvalidateRect(r);
	SetPos(aNewX, aNewY);
	InvalidateRect(r);
	InvalidateFully();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Dialog::KeyDown(int theKey)
{	
	if(Container::KeyDown(theKey))
		return true;

	if(theKey==KEYCODE_ESCAPE && mEndOnEscape)
	{
		GetWindow()->EndDialog(mEscapeId,this);
		return true;
	}
	else if(theKey==KEYCODE_RETURN)
		return ActivateDefaultButton();
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Dialog::AllowWindowClose()
{
	Window *aWindow = GetWindow();
	bool allow = Container::AllowWindowClose();
	if(!allow)
		return false;

	if(aWindow->GetWindowManager()->GetModalComponent()==this)
	{
		if(mEndOnEscape)
			aWindow->EndDialog(mEscapeId);
		else
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Dialog::ActivateDefaultButton()
{
	if(mDefaultButton.get()!=NULL && !mDefaultButton->Disabled() && mDefaultButton->IsDefaultButton())
	{
		mDefaultButton->Activate();
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Dialog::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_InputReturn)
		ActivateDefaultButton();
	else if(theEvent->mType==ComponentEvent_ButtonPressed && ((Button*)theEvent->mComponent)->ButtonFlagSet(ButtonFlag_EndDialog))
		GetWindow()->EndDialog(theEvent->mComponent->GetControlId());
	else
		Container::HandleComponentEvent(theEvent);
}

