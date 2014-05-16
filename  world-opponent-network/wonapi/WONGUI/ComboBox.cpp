#include "ComboBox.h"
#include "ListBox.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComboBox::ComboBox()
{
	SetComponentFlags(/*ComponentFlag_GrabBG |*/ ComponentFlag_WantFocus,true);
	mOnlyChildInput = true;
	mListBoxVisible = false;
	mNoType = false;
	mShowButtonWhenDisabled = true;

	mListBoxWidth = 200;
	mListBoxHeight = 150;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComboBox::~ComboBox()
{
	if(mListBox.get()!=NULL)
		mListBox->SetListener(NULL); // turn of listener
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* ComboBox::InsertString(const GUIString &theStr, int thePos /* = -1 */)
{
	if(mListArea.get()!=NULL)
	{
		ListItem *anItem = mListArea->InsertString(theStr, thePos);
		CalcListBoxSize();
		return anItem;
	}
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::InsertItem(TextListItem *theItem, int thePos)
{
	if(mListArea.get()!=NULL)
	{
		mListArea->InsertItem(theItem, thePos);
		CalcListBoxSize();
	}
	else
		TextListItemPtr aRef = theItem; // make sure item is ref counted
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::DeleteItem(int thePos)
{
	if(mListArea.get()!=NULL)
	{
		mListArea->DeleteItem(thePos);
		CalcListBoxSize();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::UpdateItem(TextListItem *theItem)
{
	if(mListArea.get()!=NULL)
		mListArea->UpdateItem(theItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::Clear(bool clearInputAsWell)
{
	mListArea->Clear();
	if(clearInputAsWell)
		mInputBox->SetText("");

	CalcListBoxSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ComboBox::GetSelItemPos()
{
	ListItem *anItem = mListArea->GetSelItem();
	if(anItem==NULL)
		return -1;
	else
		return mListArea->GetItemPos(anItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetSelItem(int theItemPos, bool fireEventOnChange)
{
	if(mListArea.get()!=NULL)
	{
		mListArea->SetSelItem(theItemPos);
		if(mListArea->GetSelItem()==NULL)
			mInputBox->SetText("",fireEventOnChange);
		else
			SetInputBoxToListSelection(false,fireEventOnChange);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetListArea(ListArea *theListArea) 
{ 
	mListArea = theListArea; 
	if(mListBox.get()!=NULL && mListBox->GetScrollArea()!=theListArea && theListArea!=NULL)
		mListArea->SetScroller(mListBox);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetListBox(Scroller *theListBox) 
{ 
	mListBox = theListBox; 
	if(mListArea.get()!=NULL && theListBox->GetScrollArea()!=mListArea && theListBox!=NULL)
		mListArea->SetScroller(mListBox);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::AddedToParent()
{
	if(mInputBox->GetParent()!=this)
		AddControls();

	Container::AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::AddControls()
{
	AddChild(mInputBox);
	AddChild(mButton);
//	AddChild(mListBox);

//	mListBox->SetVisible(false);
	mListArea->SetListFlags(ListFlag_NoAutoAdjustScrollPos,true);
	mListArea->SetComponentFlags(ComponentFlag_WantFocus,false);
	mListBox->SetListener(new ComponentListenerComp(this));
	mButton->SetButtonFlags(ButtonFlag_ActivateDown,true);
	mButton->SetComponentFlags(ComponentFlag_WantFocus,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::CalcListBoxSize()
{
	if(mListBoxVisible)
		return;

	int aListBoxWidth = mListBoxWidth;

	int aNecessaryWidth = mListArea->GetTotalWidth() + mListArea->GetLeftPad() + mListArea->GetRightPad() + (mListBox->Width() - mListArea->Width());
	if(aListBoxWidth > aNecessaryWidth)
		aListBoxWidth  = aNecessaryWidth;
	if(aListBoxWidth < Width())
		aListBoxWidth = Width();

	

	int aHeight;
//	if(mListArea->GetNumItems()==0)
//		aHeight = mListArea->GetFont()->GetHeight()/2+4;
//	else
		aHeight = mListArea->GetTotalHeight()+4;

	if(aHeight > mListBoxHeight)
		aHeight = mListBoxHeight;

	mListBox->SetSize(aListBoxWidth,aHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SizeChanged()
{
	Container::SizeChanged();
	CalcListBoxSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetListBoxDimensions(int theWidth, int theHeight)
{
	mListBoxWidth = theWidth;
	mListBoxHeight = theHeight;
	CalcListBoxSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetListBoxWidth(int theWidth)
{
	mListBoxWidth = theWidth;
	CalcListBoxSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetListBoxHeight(int theHeight)
{
	mListBoxHeight = theHeight;
	CalcListBoxSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::GotFocus()
{
	Container::GotFocus();
	if(mFocusChild==NULL)
		mInputBox->RequestFocus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::LostFocus()
{
	Container::LostFocus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetInputBoxToListSelection(bool activeSel, bool fireEvent)
{
	TextListItem *anItem = (TextListItem*)mListArea->GetSelItem();
	if (anItem != NULL)
	{
		mInputBox->SetText(anItem->mText);
	
		if(anItem->mWidth > mInputBox->Width()-10)
			mInputBox->SetSel(anItem->mText.length(),0);			
		else
			mInputBox->SetSel();
	
		if(fireEvent)
			FireEvent(new ComboSelChangedEvent(this,anItem,activeSel));
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetText(const GUIString &theText, bool fireEvent)
{
	mInputBox->SetText(theText, fireEvent);
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetTextAndSelItem(const GUIString &theText, bool fireEventOnChange)
{
	if(mListArea.get()!=NULL)
	{
		int aNumItems = mListArea->GetNumItems();
		for(int i=0; i<aNumItems; i++)
		{
			TextListItem *anItem = (TextListItem*)mListArea->GetItem(i);
			if(anItem!=NULL && anItem->mText==theText)
			{
				SetSelItem(i,fireEventOnChange);
				return;
			}
		}
	}

	SetText(theText, fireEventOnChange); // couldn't find it, just set the text in the input box
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::ShowList()
{
	if(!mListBoxVisible)
	{
//		SetComponentFlags(ComponentFlag_AllowModalInput,true);

		CalcListBoxSize();

		Window *aWindow = GetWindow();
		int x = 0, y = Height(); //int x = mInputBox->Left() ,y = mInputBox->Bottom();
		ChildToRoot(x,y);
		int aFlags = PopupFlag_StandardPopup;
		aFlags &= ~PopupFlag_WantInput;
		aFlags &= ~PopupFlag_EndOnEscape;
		PopupParams aParams(mListBox,aFlags,this,x,y);
		aParams.mInputComponent = this;
		aParams.mAvoidRect.y = y-Height(); //y-mInputBox->Height();
		aParams.mAvoidRect.height = Height(); //mInputBox->Height();

		TextListItem *anItem = (TextListItem*)mListArea->GetSelItem();
		if(anItem!=NULL)
		{
			if(anItem->mText!=mInputBox->GetText())
			{
				int i;
				for(i=0; i<mListArea->GetNumItems(); i++) // see if the item is anywhere in the list
				{
					anItem = (TextListItem*)mListArea->GetItem(i);
					if(anItem->mText==mInputBox->GetText())
					{
						mListArea->SetSelItem(i,false);
						mListArea->EnsureVisible(anItem);
						break;
					}
				}

				if(i==mListArea->GetNumItems())
					mListArea->SetSelItem(-1,false);
			}
			else
				mListArea->EnsureVisible(anItem);
		}

		aWindow->DoPopup(aParams);
		mListBoxVisible = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mComponent==mButton)
	{
		if(!mListBoxVisible)
		{
			mUndoStr = mInputBox->GetText();
			ShowList();
		}
		else
		{
			mListBoxVisible = false;
			GetWindow()->EndPopup(mListBox);
		}
	
/*		mListBox->SetVisible(!mListBox->IsVisible());
		if(mListBox->IsVisible())
			mUndoStr = mInputBox->GetText();

		Invalidate();*/
	}
	else if(theEvent->mType==ComponentEvent_ListItemClicked)
	{
		ListItemClickedEvent *anEvent = (ListItemClickedEvent*)theEvent;
		if(anEvent->mButton==MouseButton_Left)
		{
			SetInputBoxToListSelection(true);
			mListBoxVisible = false;
			GetWindow()->EndPopup(mListBox);
//			mListBox->SetVisible(false);
//			Invalidate();
		}
	}
	else if(theEvent->mType==ComponentEvent_EndPopup)
	{
//		SetComponentFlags(ComponentFlag_AllowModalInput,false);
		if(mListBoxVisible)
		{
//			mInputBox->SetText(mUndoStr);
			mListBoxVisible = false;
		}
	}
	else if(theEvent->mType==ComponentEvent_InputTextChanged)
	{
		FireEvent(ComponentEvent_InputTextChanged,true);
	}
	else if(theEvent->mType==ComponentEvent_InputReturn)
	{
		FireEvent(ComponentEvent_InputReturn,true);
	}
	else
		Container::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::MouseDown(int x, int y, MouseButton theButton)
{
	Container::MouseDown(x,y, theButton);
	if(theButton==MouseButton_Left)
	{
		if(mMouseChild==mInputBox && mNoType)
			mButton->Activate();
			
		RequestFocus();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComboBox::MouseWheel(int theAmount)
{
	if(mListBoxVisible)
		return mListArea->MouseWheel(theAmount);
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComboBox::KeyDown(int theKey)
{
	if(theKey==KEYCODE_DOWN && !mListBoxVisible)
	{
		mUndoStr = mInputBox->GetText();
		SetInputBoxToListSelection();
		ShowList();
	}
	else if(mListBoxVisible && (theKey==KEYCODE_DOWN  || theKey==KEYCODE_UP || theKey==KEYCODE_PGUP || theKey==KEYCODE_PGDN))
	{
		mListArea->KeyDown(theKey);
		SetInputBoxToListSelection();
	}
	else if(mListBoxVisible && theKey==KEYCODE_RETURN)
	{
		SetInputBoxToListSelection(true);
		mButton->Activate();
	}
	else if(theKey==KEYCODE_ESCAPE && mListBoxVisible)
	{
		mInputBox->SetText(mUndoStr);
		mButton->Activate();
	}
	else
		return Container::KeyDown(theKey);

	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::SetNoType(bool noType)
{
	if(noType==mNoType)
		return;

	mNoType = noType;
	mInputBox->SetComponentFlags(ComponentFlag_WantFocus,!noType);
	mInputBox->SetAllowSelection(!noType);
	if(noType && mFocusChild==mInputBox)
		ChildRequestFocus(NULL);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBox::Enable(bool isEnabled)
{
	if(isEnabled==!Disabled())
		return;

	Container::Enable(isEnabled);
	mInputBox->Enable(isEnabled);
	mButton->Enable(isEnabled);
	if(!isEnabled && mListBoxVisible)
	{
		mListBoxVisible = false;
		GetWindow()->EndPopup(mListBox);
	}

	if(!mShowButtonWhenDisabled)
	{
		mButton->SetVisible(isEnabled);
		RecalcLayout();
	}

}
