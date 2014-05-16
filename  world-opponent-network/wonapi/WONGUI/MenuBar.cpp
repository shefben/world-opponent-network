#include "MenuBar.h"
#include "MSControls.h"
#include "SimpleComponent.h"
#include "Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MenuItem::MenuItem(const GUIString &theText, ComponentListener *theListener) : 
	TextListItem(theText), mListener(theListener)
{
	mDisabled  = false;
	mMenuId = 0;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuItem::CalcDimensions(ListArea *theListArea)
{
	TextListItem::CalcDimensions(theListArea);
	mHeight += 4;
	mWidth += 32;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuItem::Paint(Graphics &g, ListArea *theListArea)
{
	int aColor = mColor;

	int x = 2;
	if(mIcon.get()!=NULL)
	{
		Font *aFM = mFont;
		int y = (mHeight - mIcon->GetHeight())/2;
		if(y < 0)
			y = 0;

		g.DrawImage(mIcon, x, y);
	}


	if(mSelected)
	{		
		g.SetColor(theListArea->GetSelColor().GetBackgroundColor(g));
		int aWidth = theListArea->GetPaintColumnWidth();
		if(!theListArea->ListFlagSet(ListFlag_FullRowSelect))
			aWidth = mWidth;

		int anXPos = 0;
		if(mIcon.get()!=NULL)
			anXPos = 20;

		g.FillRect(anXPos,0,aWidth,mHeight);

		if(mIcon.get()!=NULL)
		{
			g.ApplyColorSchemeColor(StandardColor_3DHilight);
			int x = 0, y = 0;
			int aBoxWidth = 20, aBoxHeight = mHeight;
			g.DrawRect(x,y,aBoxWidth,aBoxHeight);
			g.ApplyColorSchemeColor(StandardColor_3DShadow);
			g.DrawLine(x+aBoxWidth-1,y,x+aBoxWidth-1,y+aBoxHeight-1);
			g.DrawLine(x,y+aBoxHeight-1,x+aBoxHeight-1,y+aBoxHeight-1);
		}

		aColor = theListArea->GetSelColor().GetForegroundColor(g,mColor);
	}

	x+=20;
	g.SetColor(aColor);
	g.SetFont(mFont);
	int y = (mHeight - mFont->GetHeight())/2;
	if(mDisabled)
	{
		g.SetColor(0xFFFFFF);
		if(!mSelected)
		{
			g.DrawString(mText,x+1,y+1);
			g.SetColor(0x848484);
		}
		else
			g.SetColor(0xD0D0D0);

		g.DrawString(mText,x,y);
	}
	else
		g.DrawString(mText, x, y);

	if(HasSubMenu())
	{
		int width = theListArea->GetScrollArea().Width();
		int height = mHeight;
		bool disabled = mDisabled && !mSelected;
		
		int x = width-8, y = height/2;
		for(int i=0; i<4; i++)
		{
			g.DrawLine(x+(3-i), y-i, x+(3-i), y+i);	
			if(disabled && i<3)
			{
				g.SetColor(0xFFFFFF);
				g.DrawLine(x+(3-i),y+i+1,x+(3-i),y+i+2);
				g.SetColor(0x848484);
			}
		}

		if(disabled)
			g.SetPixel(x+4,y+1,0xFFFFFF);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MenuBarItem::MenuBarItem()
{
	mState = MenuBarState_None;
	mFont = GetNamedFont("Menu");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBarItem::SetMenuBarState(MenuBarState theState)
{
	mState = theState;
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBarItem::SetText(const GUIString &theText)
{
	mText = theText;
	SetSize(mFont->GetStringWidth(mText)+10,mFont->GetHeight()+4);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBarItem::SetPopup(PopupMenu *thePopup)
{
	mPopup = thePopup;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBarItem::Paint(Graphics &g)
{
	Component::Paint(g);

	bool down = mState==MenuBarState_Down;
	bool over = mState==MenuBarState_Up;
	int offset = down?1:0;

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.FillRect(0,0,Width(),Height());

	if(down || over)
	{
		g.SetColor(down?g.GetColorSchemeColor(StandardColor_3DShadow):g.GetColorSchemeColor(StandardColor_3DHilight));
		g.DrawLine(0,0,Width()-1,0);
		g.DrawLine(0,0,0,Height()-1);
	
		g.SetColor(over?g.GetColorSchemeColor(StandardColor_3DShadow):g.GetColorSchemeColor(StandardColor_3DHilight));
		g.DrawLine(0,Height()-1,Width()-1,Height()-1);
		g.DrawLine(Width()-1,0,Width()-1,Height()-1);
	}

	int x = 3;
	int y = (Height() - mFont->GetHeight())/2;
	g.SetFont(mFont);
	g.SetColor(g.GetColorSchemeColor(StandardColor_ButtonText));
	g.DrawString(mText,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PopupMenu::PopupMenu()
{
	mFont = GetNamedFont("Menu");
	SetListFlags(ListFlag_SelOver | ListFlag_SelOnlyWhenOver | ListFlag_KeyMoveWrapAround, true);
	SetBackground(new MSPopup);
	SetTextColor(ColorScheme::GetColorRef(StandardColor_MenuText));
	mMouseUpCount = 0;
	mTimerAcc = 0;
	mKeyboardMode = false;
	SetBorderPadding(4,4,4,4);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::Reset()
{
	mCurSubMenu = NULL;
	SetSelItem(-1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool PopupMenu::IsShowing()
{
	return GetRoot()!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::DoPopup(Window *theOwnerWindow, Component *theOwnerComponent, int x, int y)
{
	int aFlags = PopupFlag_StandardPopup;
	aFlags &= ~PopupFlag_FireEventOnEnd; // prevent menubar from getting a bogus event (this call is not used by the menubar)
	theOwnerWindow->DoPopup(PopupParams(this,aFlags,theOwnerComponent,x,y));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MenuItem* PopupMenu::AddItem(const GUIString &theText, int theId)
{
	return AddItem(theText,NULL,theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MenuItem* PopupMenu::AddItem(const GUIString &theText, ComponentListener *theListener, int theId)
{
	return AddItem(theText,NULL,theListener,theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MenuItem* PopupMenu::AddItem(const GUIString &theText, NativeImage *theIcon, ComponentListener *theListener, int theId)
{
	MenuItemPtr anItem = new MenuItem(theText,theListener);
	anItem->mMenuId = theId;
	anItem->mIcon = theIcon;
	InsertItem(anItem);
	SetSizeEqualToTotal();
	return anItem;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PopupMenu* PopupMenu::AddSubMenu(const GUIString &theText, PopupMenu *theSubMenu)
{
	if(theSubMenu==NULL)
		theSubMenu = new PopupMenu;

	MenuItemPtr anItem = new MenuItem(theText);
	anItem->mSubMenu = theSubMenu;
	theSubMenu->SetListener(new ComponentListenerComp(this));

	InsertItem(anItem);
	SetSizeEqualToTotal();
	return theSubMenu;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::EndSubMenu()
{
	if(mCurSubMenu.get()!=NULL)
	{
		GetWindow()->EndPopup(mCurSubMenu);

		if(!mKeyboardMode)
			SetListFlags(ListFlag_SelOnlyWhenOver,true);

		mCurSubMenu = NULL;
		mCurSubMenuItem = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::CheckSubMenu()
{
	mTimerAcc = 0;
	MenuItem *anItem = dynamic_cast<MenuItem*>(GetSelItem());
	if(anItem==NULL)
	{
		if(mCurSubMenu.get()!=NULL)
			EndSubMenu();

		return;
	}

	PopupMenu *aSubMenu = anItem->mSubMenu;
	if(mCurSubMenu.get()==aSubMenu)
		return;

	EndSubMenu();

	if(anItem->HasSubMenu() && !anItem->IsDisabled())
	{
		SetListFlags(ListFlag_SelOnlyWhenOver,false);

		mCurSubMenu = aSubMenu;
		mCurSubMenuItem = anItem;
		int x = GetScrollArea().Width()-2,y = anItem->my;
		ScrollUntranslate(x,y);
		ChildToRoot(x,y);

		int aFlags = PopupFlag_StandardPopup | PopupFlag_Additional;
		aFlags &= ~PopupFlag_EndOnEscape;

		aSubMenu->Reset();
		PopupParams aParams(aSubMenu,aFlags,this,x,y);
		aParams.mAvoidRect = WONRectangle(x-GetScrollArea().Width()+2,0,GetScrollArea().Width()-2,0);
		GetWindow()->PostEvent(new DoPopupEvent(aParams));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mComponent==mCurSubMenu && theEvent->mType==ComponentEvent_ListSelChanged)
	{
		if(mCurSubMenu->GetSelItem()!=NULL)
			SetSelItem(GetItemPos(mCurSubMenuItem));
//		else
//			SetListFlags(ListFlag_SelOnlyWhenOver,true);
	}
	else
		ListArea::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool PopupMenu::TimerEvent(int theDelta)
{
	if(mKeyboardMode)
		return false;

	mTimerAcc+=theDelta;
	if(mTimerAcc>=350)
	{
		CheckSubMenu();	
		return false;
	}
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::ListSelChangedHook()
{
	mTimerAcc = 0;
//	CheckSubMenu();
	if(!mKeyboardMode)
	{
		if(mCurSubMenu.get()!=NULL && GetSelItem()!=mCurSubMenuItem)
		{
			mCurSubMenu->SetSelItem(-1);
		//	mCurSubMenu->EndSubMenu();
		}

		RequestTimer(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::AddSeperator()
{
	InsertItem(new ComponentListItem(new MSSeperator, true, false));
	SetSizeEqualToTotal(100);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool PopupMenu::TryKeyDown(int theKey)
{
	mKeyboardMode = true;
	if(mCurSubMenu.get()!=NULL)
	{
		if(mCurSubMenu->TryKeyDown(theKey))
			return true;
		else if(theKey==KEYCODE_LEFT || theKey==KEYCODE_ESCAPE)
		{
			EndSubMenu();
			return true;
		}
		else if(theKey==KEYCODE_RIGHT && mCurSubMenu->GetSelItem()==NULL)
		{
			mCurSubMenu->SetSelItem(0);
			return true;
		}
		else
			return false;
	}
	else if(theKey==KEYCODE_RIGHT)
	{
		CheckSubMenu();
		if(mCurSubMenu.get()!=NULL)
		{
			mCurSubMenu->SetSelItem(0);
			return true;
		}
		else
			return false;
	}
	else if(theKey==KEYCODE_UP || theKey==KEYCODE_DOWN)
	{
		ListArea::KeyDown(theKey);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::MouseMove(int x, int y)
{
	if(mKeyboardMode)
	{
		if(x==mLastX && y==mLastY)
			return;
		else
		{
			mKeyboardMode = false;
			SetListFlags(ListFlag_SelOnlyWhenOver,mCurSubMenu.get()==NULL);
		}
	}

	mLastX = x;
	mLastY = y;
	ListArea::MouseMove(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::MouseDown(int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left)
	{
		mMouseUpCount = 0;
		Window *aWindow = GetWindow();
		ChildToRoot(x,y);
		aWindow->PostEvent(new MouseEvent(InputEvent_MouseUp,x,y,aWindow->GetKeyMod(),theButton));
		return;
	}
	ListArea::MouseDown(x,y,theButton);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PopupMenu::MouseUp(int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left)
	{
		mMouseUpCount++;
		if(mMouseUpCount<2)
			return; // swallow fake mouse up
	}

	ListArea::MouseUp(x,y,theButton);
	return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuItemSelectedEvent::Deliver()
{
	if(mItem->mListener.get()!=NULL)
		mItem->mListener->HandleComponentEvent(this);
	else
		ComponentEvent::Deliver();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MenuBar::MenuBar()
{
	mState = MenuBarState_None;
	mCurItem = NULL;
	mBackground.SetColor(ColorScheme::GetColorRef(StandardColor_3DFace));

	SetComponentFlags(ComponentFlag_WantFocus,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::AddSeperator(Component *theComponent)
{
	MSSeperatorPtr aSep;
	if(theComponent==NULL)
	{
		aSep = new MSSeperator(2);
		theComponent = aSep;
	}

	AddChildLayout(theComponent,CLI_SameBottom | CLI_SameWidth,this);
	AddChild(theComponent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::AddItem(const GUIString &theText, PopupMenu *thePopup)
{
	MenuBarItemPtr anItem = new MenuBarItem;
	int anId = mItemVector.size();
	if(anId > 0)
	{
		MenuBarItem *aPrev = mItemVector.back();
		anItem->SetPos(aPrev->Right()+1,aPrev->Top());
	}
	else
		anItem->SetPos(2,2);

	anItem->SetControlId(anId);
	anItem->SetText(theText);
	if(thePopup==NULL)
		thePopup = new PopupMenu;

	anItem->SetPopup(thePopup);
	if(thePopup!=NULL)
		thePopup->SetListener(new ComponentListenerComp(this));

	mItemVector.push_back(anItem);
	AddChild(anItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::CheckItem(Component *theCheck)
{
	if(mState==MenuBarState_None || mState==MenuBarState_Up)
	{
		if(mCurItem!=NULL && theCheck!=mCurItem)
			mCurItem->SetMenuBarState(MenuBarState_None);

		if(theCheck==NULL)
			return;

		mCurItem = dynamic_cast<MenuBarItem*>(theCheck);
		if(mCurItem!=NULL)
			mCurItem->SetMenuBarState(MenuBarState_Up);
	}
	else if(mState==MenuBarState_Down)
	{
		MenuBarItem *anItem = dynamic_cast<MenuBarItem*>(theCheck);
		if(anItem!=NULL && anItem!=mCurItem)
		{
			MenuBarItem *anOldCurItem = mCurItem;
			mCurItem = anItem;
			if(anOldCurItem!=NULL)
			{
				anOldCurItem->SetMenuBarState(MenuBarState_None);
				if(anOldCurItem->GetPopup()!=NULL)
					GetWindow()->EndPopup(anOldCurItem->GetPopup());
			}

			anItem->SetMenuBarState(MenuBarState_Down);
			PopupMenu *aPopup = mCurItem->GetPopup();
			if(aPopup!=NULL)
			{
				int x = mCurItem->Left();
				int y = mCurItem->Bottom();
				ChildToRoot(x,y);

//				SetComponentFlags(ComponentFlag_AllowModalInput,true);
				int aFlags = PopupFlag_StandardPopup;
				aFlags &= ~PopupFlag_WantInput;
				aFlags &= ~PopupFlag_EndOnEscape;

				aPopup->Reset();
				PopupParams aParams(aPopup,aFlags,this,x,y);
				aParams.mInputComponent = this;
				aParams.mAvoidRect = WONRectangle(0,y-mCurItem->Height(),0,mCurItem->Height());
//				GetWindow()->DoPopup(aParams);
				GetWindow()->PostEvent(new DoPopupEvent(aParams));
			}
		}
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_EndPopup && mState==MenuBarState_Down)
	{
		if(mCurItem!=NULL && mCurItem->GetPopup()==theEvent->mComponent)
		{
			mState = MenuBarState_None;
			if(GetWindow()->MouseIsInClientArea())
				MouseAt();
			else
				MouseExit();
		}		
	}
	else if(theEvent->mType==ComponentEvent_ListItemUpClicked)
	{
		ListItemClickedEvent *anEvent = (ListItemClickedEvent*)theEvent;
		MenuItem *anItem = dynamic_cast<MenuItem*>(anEvent->mItem.get());
		if(anItem!=NULL && anItem->IsSelectable())
		{
			FireEvent(new MenuItemSelectedEvent(theEvent->mComponent,anItem));
			GetWindow()->EndPopup();
		}
	}
	else
		Container::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::MouseMove(int x, int y)
{
	Container::MouseMove(x,y);
	if(x!=mLastX || y!=mLastY)
	{
		mLastX = x;
		mLastY = y;
		CheckItem(mMouseChild);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MenuBar::KeyDown(int theKey)
{
	if(mCurItem==NULL)
		return false;

	if(mState==MenuBarState_Down)
	{
		PopupMenu *aPopup = mCurItem->GetPopup();
		if(aPopup->TryKeyDown(theKey))
			return true;
	}
	else 
		mState = MenuBarState_Up;

	int anId = mCurItem->GetControlId();

	if(theKey==KEYCODE_ESCAPE)
	{
		mState = MenuBarState_Up;
		if(mCurItem!=NULL)
		{
			GetWindow()->EndPopup(mCurItem->GetPopup());
			CheckItem(mCurItem);
		}
	}
	else if(theKey==KEYCODE_RIGHT)
	{
		anId++;
		if(anId>=mItemVector.size())
			anId = 0;
	}
	else if(theKey==KEYCODE_LEFT)
	{
		anId--;
		if(anId<0)
			anId = mItemVector.size()-1;
	}
	else if(mState==MenuBarState_Up && (theKey==KEYCODE_RETURN || theKey==KEYCODE_DOWN))
	{
		mCurItem = NULL;
		mState = MenuBarState_Down;
	}

	CheckItem(mItemVector[anId]);
	if(mState==MenuBarState_Down && mCurItem!=NULL)
	{
		PopupMenu *aPopup = mCurItem->GetPopup();
		if(aPopup!=NULL)
			aPopup->SetSelItem(0);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::MouseDown(int x, int y, MouseButton theButton)
{
	Container::MouseDown(x,y,theButton);
	if(theButton!=MouseButton_Left)
		return;

	RequestFocus();
	if(dynamic_cast<MenuBarItem*>(mMouseChild)!=NULL)
	{
		if(mState==MenuBarState_Down)
		{
			mState = MenuBarState_Up;
			if(mCurItem!=NULL)
				GetWindow()->EndPopup(mCurItem->GetPopup());
		}
		else
		{
			mCurItem = NULL;
			mState = MenuBarState_Down;
			Window *aWindow = GetWindow();
			ChildToRoot(x,y);
			aWindow->PostEvent(new MouseEvent(InputEvent_MouseUp,x,y,aWindow->GetKeyMod(),theButton));
		}


		CheckItem(mMouseChild);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::MouseDrag(int x, int y)
{
	if(mState==MenuBarState_Down)
	{
		MouseAt(x,y);
		CheckItem(mMouseChild);
	}
	else
		Container::MouseDrag(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::MouseUp(int x, int y, MouseButton theButton)
{
	Container::MouseUp(x,y,theButton);
	if(theButton!=MouseButton_Left)
		return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::MouseExit()
{
	Container::MouseExit();
	CheckItem(NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MenuBar::Paint(Graphics &g)
{
	if(IsSelfInvalid())
		mBackground.Paint(g,0,0,Width(),Height(),0,0);
	
	Container::Paint(g);
}
