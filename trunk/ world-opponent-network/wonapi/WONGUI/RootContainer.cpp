#include "RootContainer.h"
#include "Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RootContainer::RootContainer(Window *theWindow)
{
	mModalComponent = NULL;
	mWindow = theWindow;
	mCursorComponent = NULL;
	SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_Arrow));
	SetComponentFlags(ComponentFlag_WantFocus | ComponentFlag_IsVisibleToRoot,true);

	mAllowDrag = true;
	mDragX = mDragY = 0;
	mDragging = false;
	mIsolateTabFocus = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::Reset()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::AddBG(BackgroundGrabber *theGrabber)
{
	if(theGrabber==NULL || !theGrabber->NeedDrawBG())
		return;

	mBGStructMap[theGrabber->mImageOrder] = BGStruct(theGrabber->mImageX,theGrabber->mImageY,theGrabber->mBGImage, theGrabber->mClipRect);
	theGrabber->Clear();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::SetModalComponent(Component *theComponent)
{
	if(mModalComponent!=NULL) // Was already in modal
	{
		mModalComponent->SetComponentFlags(ComponentFlag_AllowModalInput, false);
		if(theComponent==NULL) // No longer in modal
			LeaveModal();
	}
	else if(theComponent!=NULL) // Entering Modal
	{
		theComponent->SetComponentFlags(ComponentFlag_AllowModalInput,true);
		EnterModal();		
	}

	mModalComponent = theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* RootContainer::FindChildAt(int x, int y)
{
	Component *aComponent = Container::FindChildAt(x,y);
/*	if(mModalComponent!=NULL && aComponent!=mModalComponent)
		aComponent = NULL;
	if(!mPopupSet.empty() && mPopupSet.find(aComponent)==mPopupSet.end())
		aComponent = NULL;*/

	return aComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::SizeChanged()
{
	Container::SizeChanged();
	mInvalidRect = mBounds;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::PrePaint(Graphics &g)
{
//	g.PushClipRect(mInvalidRect.x, mInvalidRect.y, mInvalidRect.width, mInvalidRect.height);
	Container::PrePaint(g);
	BGStructMap::reverse_iterator anItr = mBGStructMap.rbegin();
	while(anItr!=mBGStructMap.rend())
	{
		BGStruct &aStruct = anItr->second;
		WONRectangle anImageRect(aStruct.mImageX,aStruct.mImageY,aStruct.mImage->GetWidth(),aStruct.mImage->GetHeight());
		anImageRect.DoIntersect(aStruct.mClipRect);
		g.DrawImage(aStruct.mImage, anImageRect.x, anImageRect.y, anImageRect.x - aStruct.mImageX, anImageRect.y - aStruct.mImageY, anImageRect.width, anImageRect.height); 

/*		WONRectangle &aClip = aStruct.mClipRect;
		g.PushClipRect(aClip.x,aClip.y,aClip.width,aClip.height);
		g.DrawImage(aStruct.mImage,aStruct.mImageX,aStruct.mImageY);
		g.PopClipRect();*/
		++anItr;
	}
	mBGStructMap.clear();
//	g.PopClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::Paint(Graphics &g)
{
	g.SetColorScheme(GetColorScheme());
	g.PushClipRect(mInvalidRect.x, mInvalidRect.y, mInvalidRect.width, mInvalidRect.height);
	Container::Paint(g);
	g.PopClipRect();

	mInvalidRect.x = mInvalidRect.y = mInvalidRect.width = mInvalidRect.height = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RootContainer::InvalidateUp(Component *theChild)
{
	return Container::InvalidateUp(theChild);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::InvalidateRect(WONRectangle &theRect)
{
	mInvalidRect.DoUnion(theRect);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RootContainer::KeyDown(int theKey)
{
	Container::KeyDown(theKey);

	if(mKeyboardListener.get()!=NULL)
		mKeyboardListener->KeyEvent(theKey,0);	

	return true;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RootContainer::KeyUp(int theKey)
{
	Container::KeyUp(theKey);
	if(mKeyboardListener.get()!=NULL)
		mKeyboardListener->KeyEvent(theKey,1);

	return true;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RootContainer::KeyChar(int theKey)
{
	Container::KeyChar(theKey);
	if(mKeyboardListener.get()!=NULL)
		mKeyboardListener->KeyEvent(theKey,2);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::MouseMove(int x, int y)
{
	Container::MouseMove(x,y);
	if(mCursorComponent!=NULL)
	{
		int mx,my;
		mCurrentCursor->GetComponent(mx,my);
		x-=mx; y-=my; 

		mCursorComponent->InvalidateFully();
		mCursorComponent->SetPos(x,y);
		WONRectangle aRect(x,y,mCursorComponent->Width(),mCursorComponent->Height());
		InvalidateRect(aRect);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::MouseDrag(int x, int y)
{
	if(!mDragging)
	{
		Container::MouseDrag(x,y);
		if(mCursorComponent!=NULL)
		{
			int mx,my;
			mCurrentCursor->GetComponent(mx,my);
			x-=mx; y-=my; 

			mCursorComponent->InvalidateFully();
			mCursorComponent->SetPos(x,y);
			WONRectangle aRect(x,y,mCursorComponent->Width(),mCursorComponent->Height());
			InvalidateRect(aRect);
		}
	}
	else
	{
		int aNewX = Left() + (x - mDragX);
		int aNewY = Top() + (y - mDragY);
		if(aNewX==Left() && aNewY==Top())
			return;

		Window *aWindow = GetWindow();
		WONRectangle r;
		aWindow->GetScreenPos(r);
		aWindow->Move(r.Left() + aNewX, r.Top() + aNewY);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::MouseDown(int x, int y, MouseButton theButton)
{
	Container::MouseDown(x,y,theButton);
	if(theButton==MouseButton_Left && mAllowDrag)
	{
		Window *aWindow = GetWindow();
		Component *aMouseChild = mMouseChild;
		if(aWindow->IsOneComponentWindow() && aMouseChild!=NULL)
			aMouseChild = aMouseChild->GetMouseChild();

		if(aMouseChild!=NULL)
		{
			if(aMouseChild->ComponentFlagSet(ComponentFlag_ParentDrag))
			{
				mDragging = true;
				mDragX = x;
				mDragY = y;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::MouseEnter(int x, int y)
{
	Container::MouseEnter(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::MouseExit()
{
	Container::MouseExit();
	mDragging = false;
	if(mCursorComponent!=NULL)
		RemoveChild(mCursorComponent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::MouseUp(int x, int y, MouseButton theButton)
{
	Container::MouseUp(x,y,theButton);

	if(theButton==MouseButton_Left)
		mDragging = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::RemoveChild(Component *theChild)
{
/*	if(theChild==mCursorComponent)
	{
		Graphics &g = GetWindow()->GetGraphics();
		g.Translate(theChild->Left(),theChild->Top());
		theChild->Invalidate();
		theChild->PrePaint(g);
		g.Translate(-theChild->Left(),-theChild->Top());

		mCursorComponent = NULL;
		theChild->SetParent(NULL);
		mChildList.erase(theChild->GetChildListItr());	
	}
	else Container::RemoveChild(theChld);*/
	if(theChild==mCursorComponent)
		mCursorComponent = NULL;

	Container::RemoveChild(theChild);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::SetCursor(Cursor *theCursor)
{
	Container::SetCursor(theCursor);
	if(theCursor==NULL)
		theCursor = GetDefaultCursor();

	Component *aComponent = NULL;
	int mx = 0, my = 0;
	if(theCursor!=NULL)
		aComponent = theCursor->GetComponent(mx,my);

	if(mCursorComponent!=aComponent)
	{
		if(mCursorComponent!=NULL)
			RemoveChild(mCursorComponent);

		mCursorComponent = aComponent;
		if(aComponent!=NULL)
		{
			if(aComponent->GetParent()!=NULL)
			{
				aComponent->GetParent()->RemoveChild(aComponent);
				BackgroundGrabber *aBG = aComponent->GetGrabBG();
				if(aBG!=NULL)
				{
					aBG->Clear();
					aBG->mBGImage = NULL; // this makes it so the cursor doesn't draw the new background on the old window
				}
			}

			aComponent->SetComponentFlags(ComponentFlag_GrabBG | ComponentFlag_NoInput | ComponentFlag_AlwaysOnTop, true);
			aComponent->SetVisible(true);
			
			int x = GetWindow()->GetMouseX()-mx; 
			int y = GetWindow()->GetMouseY()-my; 
			aComponent->SetPos(x,y);

			AddChild(aComponent);
			Component *aTop = mChildList.back();
			if(aTop!=aComponent)
				aComponent->EnsureOnTopOf(aTop);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::AddPopup(Component *theComponent)
{
	theComponent->SetComponentFlags(ComponentFlag_AllowModalInput,true);
	AddChild(theComponent);
	EnterModal();

	theComponent->RequestFocus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootContainer::RemovePopup(Component *theComponent)
{
	theComponent->SetComponentFlags(ComponentFlag_AllowModalInput,false);
	RemoveChild(theComponent);
	LeaveModal();
}


