#include "Window.h"
#include "Font.h"
#include "WindowManager.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::Window::Window()
{
	mWindowManager = WindowManager::GetDefaultWindowManager();
	mParent = NULL;

	mRootContainer = new RootContainer(this);
	mCurrentCursor = NULL;
	mMouseIsDown[0] = mMouseIsDown[1] = mMouseIsDown[2] = false;
	mMouseIsInClientArea = false;
	mNativeCursorShowing = true;
	mDetectDoubleClick = false;

	mMouseX = mMouseY = mKeyMod = 0;
	mLastMoveTick = 0;
	mLastCheckCursorTick = 0;

	mMinWidth = 0;
	mMinHeight = 0;
	mMaxWidth = 64000;
	mMaxHeight = 64000;

	mNeedResize = false;
	mNewWidth = mNewHeight = 0;
	mResizeDelay = 0;
	mLastResizeTick = 0;

	mCreateFlags = 0;
	mWindowState = WindowState_Closed;
	mIsShowing = false;

	mIsPopup = false;
	mIsOneComponentWindow = false;
	mSuspendRootResize = 0;
	mAllowCloseFunc = NULL;
	mMinimizeOnResolutionRestore = false;

	mTitle = "Sierra App";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::Window::~Window()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::Invalidate()
{
	mRootContainer->Invalidate();
	WONRectangle &r = mRootContainer->GetInvalidRect();
	r.DoUnion(mRootContainer->GetBounds()); // make sure entire window is marked as invalid
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetParent(Window *theParent)
{
	mParent = theParent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::Window* WONAPI::Window::GetDefaultWindow()
{
	return WindowManager::GetDefaultWindowManager()->GetDefaultWindow();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::RawImage32Ptr WONAPI::Window::CreateRawImage32(int theWidth, int theHeight)
{
	return GetDefaultWindow()->GetDisplayContext()->CreateRawImage32(theWidth,theHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::RawImage8Ptr WONAPI::Window::CreateRawImage8(int theWidth, int theHeight, Palette *thePalette)
{
	return GetDefaultWindow()->GetDisplayContext()->CreateRawImage8(theWidth,theHeight,thePalette);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::RawImage1Ptr WONAPI::Window::CreateRawImage1(int theWidth, int theHeight)
{
	return GetDefaultWindow()->GetDisplayContext()->CreateRawImage1(theWidth,theHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontPtr WONAPI::Window::GetFont(const FontDescriptor &theDescriptor)
{
	FontMap::iterator anItr = mFontMap.insert(FontMap::value_type(theDescriptor, NULL)).first;
	FontPtr aFont = anItr->second;
	if(aFont.get()==NULL)
	{
		aFont = GetNativeFont(theDescriptor);
		anItr->second = aFont;
	}

	return aFont;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool gWindowSkipDecodeImages = false;
void WONAPI::Window::SetSkipDecodeImages(bool skip)
{
	gWindowSkipDecodeImages = skip;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr WONAPI::Window::DecodeImage(const char *theFilePath)
{
	if(gWindowSkipDecodeImages)
	{
		static NativeImagePtr anImage;
		if(anImage.get()==NULL)
		{
			anImage = CreateImage(10,10);
			anImage->GetGraphics().SetColor(0);
			anImage->GetGraphics().FillRect(0,0,20,20);
		}
		
		return anImage;
	}


	return mWindowManager->DecodeImage(mDisplayContext,theFilePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr WONAPI::Window::DecodeImageRaw(const char *theFilePath)
{
	if(gWindowSkipDecodeImages)
	{
		return mDisplayContext->CreateRawImage8(10,10).get(); //new RawImage8(10,10);
	}
	return mWindowManager->DecodeImageRaw(theFilePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr WONAPI::Window::CreateImage(int theWidth, int theHeight)
{
	return mDisplayContext->CreateImage(theWidth,theHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImage* WONAPI::Window::GetOffscreenImage()
{
	return mOffscreenImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Graphics& WONAPI::Window::GetGraphics()
{
	return mOffscreenImage->GetGraphics();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::Create(const WONRectangle& theWindowRectR)
{
	if(mIcon.get()==NULL)
		mIcon = mWindowManager->GetDefaultWindowIcon();

	mWindowState = WindowState_Open;

	if(mParent!=NULL)
		mParent->mChildWindowSet.insert(this);
	
	mRootContainer->SetSize(theWindowRectR.Width(), theWindowRectR.Height());

	mWindowManager->AddWindow(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::ClosePrv()
{
	ShowNativeCursor(true); // make sure to restore the cursor before closing
	mWindowState = WindowState_Closed;
	mIsShowing = false;

	if(mParent!=NULL)
		mParent->mChildWindowSet.erase(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::Window::CanClose()
{
	bool childrenCanClose = true;
	for(ChildWindowSet::iterator anItr = mChildWindowSet.begin(); anItr!=mChildWindowSet.end(); ++anItr)
	{
		Window *aChild = *anItr;
		if(!aChild->CanClose())
			childrenCanClose = false;
	}

	if(!childrenCanClose)
		return false;

	if(!mRootContainer->AllowWindowClose())
		return false;

	if(mAllowCloseFunc!=NULL && !mAllowCloseFunc(this))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::Window::Close()
{
	if(!IsOpen())
		return true;

	if(!CanClose())
		return false;

	mWindowState = WindowState_Closing;
	PostEvent(new InputEvent(InputEvent_WindowClose));
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::CheckResize(DWORD theTick)
{
	if(mNeedResize && theTick-mLastResizeTick>mResizeDelay && mSuspendRootResize<=0)
	{
		NativeImagePtr anImage = CreateImage(mNewWidth,mNewHeight);
		Graphics &g = anImage->GetGraphics();
		g.DrawImage(mOffscreenImage,0,0);
		mOffscreenImage = anImage;
		mRootContainer->SetSize(mNewWidth, mNewHeight);
		mRootContainer->Invalidate();
		mNeedResize = false;
		mLastResizeTick = GetTickCount();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SuspendRootResize(bool suspend)
{
	if(suspend)
		mSuspendRootResize++;
	else
	{
		mSuspendRootResize--;
		if(mSuspendRootResize<=0)
		{
			mSuspendRootResize = 0;
			CheckResize(GetTickCount());
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetTitle(const GUIString &theTitle)
{
	mTitle = theTitle;
	if(mWindowState==WindowState_Open)
		SetTitleHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetTitleHook()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetIcon(NativeIcon *theIcon)
{
	mIcon = theIcon;
	if(mWindowState==WindowState_Open)
		SetIconHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetIconHook()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::CenterWindow(bool inParent)
{
	WONRectangle aScreenRect, aWindowRect;
	if(!inParent || mParent==NULL || !mParent->IsShowing())
		mWindowManager->GetScreenRect(aScreenRect);
	else
		mParent->GetScreenPos(aScreenRect);

	GetScreenPos(aWindowRect);

	int left = aScreenRect.Left() + (aScreenRect.Width() - aWindowRect.Width())/2;
	int top = aScreenRect.Top() + (aScreenRect.Height() - aWindowRect.Height())/2;
	if(left<0)
		left = 0;
	if(top<0)
		top = 0;

	Move(left,top,aWindowRect.Width(), aWindowRect.Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::Resize(int width, int height)
{
	WONRectangle aRect;
	GetScreenPos(aRect);
	Move(aRect.x,aRect.y,width,height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::Move(int x, int y, int width, int height)
{	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::GetScreenPos(WONRectangle &theRectR, bool clientArea)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetFocus()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetMouseCapture(bool on)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetNativeCursor(Cursor *theCursor)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::ShowNativeCursorHook(bool show)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetTopMostHook(bool topMost)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetTopMost(bool topMost)
{
	SetTopMostHook(topMost);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::Show(bool show)
{
	mIsShowing = show;
	if(!show)
	{
		if(MouseIsInClientArea() || mMouseIsDown[MouseButton_Left])
			HandleMouseExit();
	}

	ShowWindowHook(show);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::Minimize(bool minimize)
{
	MinimizeWindowHook(minimize);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::ShowWindowHook(bool show)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::MinimizeWindowHook(bool minimize)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::ShowNativeCursor(bool show)
{
//	if(mNativeCursorShowing==show)
//		return;

	mNativeCursorShowing = show;
	ShowNativeCursorHook(show);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::CheckSetCursor(bool force)
{
	if(!force && mRootContainer->GetCurrentCursor()==mCurrentCursor)
		return;

	mCurrentCursor = mRootContainer->GetCurrentCursor();

	Cursor* aCursor = mRootContainer->GetCurrentCursor();
	if(aCursor==NULL)
		aCursor = Cursor::GetStandardCursor(StandardCursor_Arrow);

	aCursor = aCursor->GetNative();
	if(aCursor!=NULL)
	{
		ShowNativeCursor(true);
		SetNativeCursor(aCursor);
	}
	else
		ShowNativeCursor(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::AddComponent(Component *theComponent)
{
	mRootContainer->AddChild(theComponent);
//	mRootContainer->Invalidate();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::RemoveComponent(Component *theComponent)
{
	mRootContainer->RemoveChild(theComponent);
//	mRootContainer->Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::SetModal(bool isModal)
{
	if(isModal)
		mRootContainer->EnterModal();
	else
		mRootContainer->LeaveModal();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONAPI::Window::DoDialog(Component *theComponent)
{
	return mWindowManager->DoDialog(this,theComponent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::EndDialog(int theResult, Component *theDialog)
{
	mWindowManager->EndDialog(theResult, this, theDialog);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::DoPopup(const PopupParams &theParams)
{
	((PopupParams&)theParams).mParent = this;
	mWindowManager->DoPopup(theParams);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::EndPopup(Component *thePopup)
{
	mWindowManager->EndPopup(thePopup);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::PostEvent(WindowEventBase *theEvent)
{
	theEvent->mWindow = this;
	mWindowManager->PostEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::Window::CheckDoubleClick(MouseButton theButton)
{
	return mButtonDoubleClick[theButton];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::Window::IsMouseDown(MouseButton theButton)
{
	return mMouseIsDown[theButton];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::Window::CheckDialog(InputEvent *theEvent)
{
/*	if(mModalWindow!=NULL && this!=mModalWindow)
	{
		if(theEvent->mType==InputEvent_GotFocus)
			mModalWindow->SetFocus();

		return true;
	}*/
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::HandleMouseExit()
{
	ShowNativeCursor(true); // show cursor when it leaves the window

	mMouseIsInClientArea = false;
	mMouseX = mRootContainer->Right()+1;
	mMouseY = mRootContainer->Bottom()+1;
	mMouseIsDown[0] = mMouseIsDown[1] = mMouseIsDown[2] = false;
	mRootContainer->MouseExit();
	mCurrentCursor = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::HandleKeyEvent(KeyEvent *theEvent)
{
	mKeyMod = theEvent->mKeyMod;
	switch(theEvent->mType)
	{
		case InputEvent_KeyDown:
			mRootContainer->KeyDown(theEvent->mKey);
			break;

		case InputEvent_KeyUp:
			mRootContainer->KeyUp(theEvent->mKey);
			break;

		case InputEvent_KeyChar:
			mRootContainer->KeyChar(theEvent->mKey);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::HandleMouseDown(MouseEvent *theEvent)
{
	int aButton = theEvent->mButtonOrAmount;
	mMouseIsDown[aButton] = true;

	// Check for DoubleClick
	bool isDoubleClick = false;
	if(theEvent->mType==InputEvent_MouseDoubleClick)
		isDoubleClick = true;
	else if(theEvent->mType==InputEvent_MouseDownCheckDoubleClick)
	{
		if(!mDetectDoubleClick)
		{
			for(int i=0; i<3; i++)
				mButtonDownTick[i] = 0;

			mDetectDoubleClick = true;
			mDoubleClickX = theEvent->mx;
			mDoubleClickY = theEvent->my;
		}

		DWORD &aPrevTick = mButtonDownTick[aButton];
		DWORD aTick = GetTickCount();
		if(aTick==0) // 0 has special meaning
			aTick = 1;
		
		if(aPrevTick!=0 && aTick-aPrevTick<250)
		{
			isDoubleClick = true;
			aPrevTick = 0;
		}
		else
			aPrevTick = aTick;
	}
	
	mButtonDoubleClick[aButton] = isDoubleClick;
	mRootContainer->MouseDown(mMouseX, mMouseY, (MouseButton)aButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::HandleMouseEvent(MouseEvent *theEvent)
{
	mMouseX = theEvent->mx;
	mMouseY = theEvent->my;
	mKeyMod = theEvent->mKeyMod;

	if(!mMouseIsInClientArea && !mMouseIsDown[MouseButton_Left])
	{
		mMouseIsInClientArea = true;
		mRootContainer->MouseEnter(mMouseX, mMouseY);
		CheckSetCursor(true);
	}

	switch(theEvent->mType)
	{		
		case InputEvent_MouseMove:
			mLastMoveTick = GetTickCount();
			if(mDetectDoubleClick && mMouseX!=mDoubleClickX || mMouseY!=mDoubleClickY)
				mDetectDoubleClick = false;

			if(mMouseIsDown[MouseButton_Left])
				mRootContainer->MouseDrag(mMouseX, mMouseY); 
			else
				mRootContainer->MouseMove(mMouseX, mMouseY);
			break;

		case InputEvent_MouseDown:
		case InputEvent_MouseDoubleClick:
		case InputEvent_MouseDownCheckDoubleClick:
			HandleMouseDown(theEvent);
			break;

		case InputEvent_MouseUp:
		{
			mLastMoveTick = 0;

			int aButton = theEvent->mButtonOrAmount;
			mMouseIsDown[aButton] = false;

			if(aButton==MouseButton_Left)
			{
				if(!mMouseIsInClientArea)
				{
					HandleMouseExit(); // Call MouseExit when dragging is done
					break;
				}
			}
			mRootContainer->MouseUp(mMouseX, mMouseY, (MouseButton)aButton);			
		}
		break;

		case InputEvent_MouseWheel:
			mRootContainer->MouseWheel(theEvent->mButtonOrAmount);
			break;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::HandleInputEvent(InputEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case InputEvent_SizeChange: 
		{
			SizeEvent *anEvent = (SizeEvent*)theEvent;
			if(anEvent->mWidth!=-1 && anEvent->mHeight!=-1)
			{
				if(mRootContainer->Width()!=anEvent->mWidth || mRootContainer->Height()!=anEvent->mHeight)
				{
					mNeedResize = true;
					mNewWidth = anEvent->mWidth;
					mNewHeight = anEvent->mHeight;
					CheckResize(GetTickCount());
				}
			}
		}
		break;
		
		case InputEvent_GotFocus:
			mRootContainer->GotFocus();
			break;

		case InputEvent_LostFocus:
			mRootContainer->LostFocus();				
			break;

		case InputEvent_MouseExit:
			mMouseIsInClientArea = false;
			if(!mMouseIsDown[MouseButton_Left])
				HandleMouseExit();
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::Window::MessageLoopHook()
{
	if(IsOpen())
	{
		bool force = false;
		if(mMouseIsInClientArea)
		{
			DWORD aTick = GetTickCount();
			if(aTick - mLastCheckCursorTick >= 1000)
			{
				force = true;
				mLastCheckCursorTick = aTick; 
			}
		}
		
		CheckSetCursor(force);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::TimerHook(DWORD theElapsed)
{
	if(mMouseIsInClientArea && mLastMoveTick!=0)
	{
		if(!mMouseIsDown[MouseButton_Left] && !mMouseIsDown[MouseButton_Middle] && !mMouseIsDown[MouseButton_Right])
		{
			DWORD anElapsed = GetTickCount() - mLastMoveTick;
			if(anElapsed >= 100)
				mRootContainer->MouseHover(anElapsed);
		}
	}

	CheckResize(GetTickCount());

	if(mRootContainer->WantTimer())
		mRootContainer->TimerEvent(theElapsed);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::Window::ReMouseMove() // simulates a mouse move to the current location
{
	PostEvent(new MouseEvent(InputEvent_MouseMove,mMouseX,mMouseY,mKeyMod));
}
