#include "WindowManager.h"
#include "GUICompat.h"
#include "BMPDecoder.h"
#include "GIFDecoder.h"
#include "WIMDecoder.h"
#include "TGADecoder.h"
#include "DelayLoadImage.h"

#include "WONCommon/Thread.h"

using namespace WONAPI;

WindowManager* WindowManager::mDefaultWindowManager = NULL;
DWORD WindowManager::mThreadId = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WindowManager::WindowManager()
{
	mThreadId = Thread::GetCurThreadId();
	mIsFullScreen = false;
	mOriginalScreenWidth = mOriginalScreenHeight = 0;
	mLastLoseFocusTick = 0;

	mDefaultWindowManager = this;
	mLastTimerTick = GetTickCount();
	mEndPopup = false;

	mDefaultWindow = NULL;
	mWindowCount = 0;
	mCurMsgLoopDepth = 0;
	mTargetMsgLoopDepth = 0;
	mDialogResult = 0;
	mLastUserInputTick = 0;
	mCacheImageCount = 0;

	mModalWindow = NULL;
	mModalComponent = NULL;
	mPopupOwnerWindow = NULL;

	mFocusWindow = NULL;
	mMouseDownWindow = NULL;

	mIsValid = true;
	mInDoPopup = false;
	mInEndPopup = false;
	mPopupFlags = 0;
	mPopupIsUp = false;
	mPopupInOwnWindowDef = true;
	mPauseEvents = 0;

	mDoMouseCapturePopupSupport = false;
	mCallingTimers = false;

	mColorScheme = new ColorScheme;
	InitColorScheme();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WindowManager::~WindowManager()
{
	CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::AddImageDecoders()
{
	mImageDecoder.AddDecoder(new BMPDecoder,"bmp");
	mImageDecoder.AddDecoder(new WIMDecoder,"wim");
	mImageDecoder.AddDecoder(new TGADecoder,"tga");

#ifdef _DEBUG
	mImageDecoder.AddDecoder(new GIFDecoder,"gif");
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiImageDecoder* WindowManager::GetImageDecoder()
{
	if(mImageDecoder.GetNumDecoders()==0)
		AddImageDecoders();

	return &mImageDecoder;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::InitColorScheme()
{
	mColorScheme->SetStandardColor(StandardColor_3DDarkShadow,0x000000);
	mColorScheme->SetStandardColor(StandardColor_3DFace,0xD0D0D0);
	mColorScheme->SetStandardColor(StandardColor_3DHilight,0xFFFFFF);
	mColorScheme->SetStandardColor(StandardColor_3DShadow,0x848484);
	mColorScheme->SetStandardColor(StandardColor_Scrollbar,0xDEDEDE);

	mColorScheme->SetStandardColor(StandardColor_ButtonText,0x000000);
	mColorScheme->SetStandardColor(StandardColor_GrayText,0x848484);
	mColorScheme->SetStandardColor(StandardColor_Hilight,0x08246B);
	mColorScheme->SetStandardColor(StandardColor_HilightText,0xFFFFFF);

	mColorScheme->SetStandardColor(StandardColor_ToolTipBack,0xFFFFDD);
	mColorScheme->SetStandardColor(StandardColor_ToolTipText,0x000000);

	mColorScheme->SetStandardColor(StandardColor_MenuBack,0xD0D0D0);
	mColorScheme->SetStandardColor(StandardColor_MenuText,0);

	mColorScheme->SetStandardColor(StandardColor_Back,0xFFFFFF);
	mColorScheme->SetStandardColor(StandardColor_Text,0x000000);

	mColorScheme->SetStandardColor(StandardColor_Link,0x0000FF);
	mColorScheme->SetStandardColor(StandardColor_LinkDown,0xFF0000);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::InitSystemColorScheme()
{	
	GetSystemColorScheme(mColorScheme);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::NotifyLostFocus()
{
	mFocusWindow = NULL;
	mLastLoseFocusTick = GetTickCount();
	if(mLastLoseFocusTick==0)
		mLastLoseFocusTick = 1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::NotifyGotFocus(Window *theWindow)
{
	mFocusWindow = theWindow;
	mLastLoseFocusTick = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::GetScreenResolution(int &theWidth, int &theHeight)
{
	GetScreenResolutionHook(theWidth,theHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::SetScreenResolution(int theWidth, int theHeight)
{
	int aWidth = 0,aHeight = 0;
	GetScreenResolutionHook(aWidth, aHeight);
	if(aWidth!=theWidth && aHeight!=theHeight)
	{
		SetScreenResolutionHook(theWidth,theHeight);
		mOriginalScreenWidth = aWidth;
		mOriginalScreenHeight = aHeight;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WindowManager::SetScreenResolutionHook(int theWidth, int theHeight)
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::GetScreenResolutionHook(int &theWidth, int &theHeight)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::RestoreScreenResolution()
{
	WindowSet::iterator anItr = mWindowSet.begin();
	while(anItr!=mWindowSet.end())
	{
		Window *aWindow = *anItr;
		if(aWindow->IsOpen() && aWindow->mMinimizeOnResolutionRestore)
			aWindow->Minimize(true);

		++anItr;
	}

	if(mOriginalScreenWidth!=0 && mOriginalScreenHeight!=0)
	{
		SetScreenResolution(mOriginalScreenWidth, mOriginalScreenHeight);
		mOriginalScreenWidth = 0;
		mOriginalScreenHeight = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::SetIsFullScreen(bool isFullScreen, int theWidth, int theHeight)
{
	mIsFullScreen = isFullScreen;
	mFullScreenWidth = theWidth;
	mFullScreenHeight = theHeight;

	if(mIsFullScreen)
		SetScreenResolution(theWidth,theHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Window* WindowManager::GetPopupWindow()
{
	if(mPopupIsUp)
		return mPopupList.front()->GetWindow();
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::SetNamedFont(const std::string &theName, Font *theFont)
{
	mNamedFontMap[theName] = theFont;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::Font* WindowManager::GetNamedFont(const std::string &theName)
{
	NamedFontMap::iterator anItr = mNamedFontMap.find(theName);
	if(anItr==mNamedFontMap.end())
		return GetDefaultFont();
	else
		return anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* WindowManager::GetDefaultFont()
{
	if(mDefaultFont.get()==NULL)
		mDefaultFont = mDefaultWindow->GetFont(FontDescriptor("Tahoma",FontStyle_Plain,8));

	return mDefaultFont;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* WindowManager::GetDefaultFontMod(int theStyle, int theSize)
{
	Font *aFont = GetDefaultFont();
	FontDescriptor aDesc = aFont->GetDescriptor();
	aDesc.mStyle = theStyle;
	if(theSize>0)
		aDesc.mSize = theSize;

	return mDefaultWindow->GetFont(aDesc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* WindowManager::GetNamedFontMod(const std::string &theName, int theStyle, int theSize)
{
	Font *aFont = GetNamedFont(theName);
	FontDescriptor aDesc = aFont->GetDescriptor();
	aDesc.mStyle = theStyle;
	if(theSize>0)
		aDesc.mSize = theSize;

	return mDefaultWindow->GetFont(aDesc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::SetCacheImages(bool cache)
{
	if(cache)
		mCacheImageCount++;
	else
	{
		mCacheImageCount--;
		if(mCacheImageCount<=0)
		{
			mCacheImageCount = 0;
			mNativeImageMap.clear();
			mRawImageMap.clear();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr WindowManager::DecodeImage(DisplayContext *theContext, const char *theFilePath)
{
	if(theContext==NULL)
		return NULL;

	if(mCacheImageCount)
	{
		NativeImageMap::iterator anItr = mNativeImageMap.insert(NativeImageMap::value_type(theFilePath,NULL)).first;
		if(anItr->second.get()==NULL)
			anItr->second = GetImageDecoder()->Decode(theContext,theFilePath);

		return anItr->second;
	}
	else
		return GetImageDecoder()->Decode(theContext,theFilePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr WindowManager::DecodeImage(const char *theFilePath)
{
	if(mDefaultWindow==NULL)
		return NULL;

	return DecodeImage(mDefaultWindow->GetDisplayContext(),theFilePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr WindowManager::DecodeDelayImage(const char *theFilePath)
{
	if(mCacheImageCount)
	{
		std::pair<DelayImageMap::iterator,bool> aRet;
		aRet = mDelayImageMap.insert(DelayImageMap::value_type(theFilePath,NULL));
		if(aRet.second)
		{
			ImageAttributes anAttr;
			if(GetImageDecoder()->GetImageAttributes(theFilePath,anAttr))
				aRet.first->second = new DelayLoadImage(theFilePath,anAttr.mWidth,anAttr.mHeight);
		}

		return aRet.first->second;
	}
	else
	{
		ImageAttributes anAttr;
		if(GetImageDecoder()->GetImageAttributes(theFilePath,anAttr))
			return new DelayLoadImage(theFilePath,anAttr.mWidth,anAttr.mHeight);
		else
			return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr WindowManager::DecodeImageRaw(const char *theFilePath)
{
	if(mCacheImageCount)
	{
		RawImageMap::iterator anItr = mRawImageMap.insert(RawImageMap::value_type(theFilePath,NULL)).first;
		if(anItr->second.get()==NULL)
			anItr->second = GetImageDecoder()->DecodeRaw(theFilePath);

		return anItr->second;
	}
	else
		return GetImageDecoder()->DecodeRaw(theFilePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::SetSoundDecoder(SoundDecoder *theDecoder)
{
	mSoundDecoder = theDecoder;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundPtr WindowManager::DecodeSound(const SoundDescriptor &theDesc)
{
	if(mSoundDecoder.get()==NULL)
		return new Sound(theDesc);
	else
		return mSoundDecoder->Decode(theDesc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::CloseAllWindows()
{
	bool hadWindows = !mWindowSet.empty();
	AutoCrit aCrit(mDataCrit);
	WindowSet::iterator anItr = mWindowSet.begin();
	while(anItr!=mWindowSet.end())
	{
		Window *aWindow = *anItr;
		aWindow->Close();
		++anItr;
	}

	aCrit.Leave();

	if(hadWindows)
		ProcessEvents();

	if(mIsFullScreen)
		RestoreScreenResolution();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontPtr WindowManager::GetDefaultFont(const FontDescriptor &theDescriptor)
{
	Window *aWindow = Window::GetDefaultWindow();
	if(aWindow==NULL)
		return NULL;
	else
		return aWindow->GetFont(theDescriptor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DisplayContext* WindowManager::GetDefaultDisplayContext()
{
	Window *aWindow = Window::GetDefaultWindow();
	if(aWindow==NULL)
		return NULL;
	else
		return aWindow->GetDisplayContext();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WindowManager::AddWindow(Window *theWindow)
{
	AutoCrit aCrit(mDataCrit);
	if(mWindowSet.empty())
		mDefaultWindow = theWindow;

	if(mWindowSet.insert(theWindow).second)
		mWindowCount++;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WindowManager::RemoveWindow(Window *theWindow)
{
	AutoCrit aCrit(mDataCrit);
		
	if(mWindowSet.erase(theWindow)>0)
	{
		mWindowCount--;
		if(mWindowCount<=0)
			mTargetMsgLoopDepth = 0;

		if(mWindowSet.empty())
			mDefaultWindow = NULL;
		else 
			mDefaultWindow = *mWindowSet.begin();

		// Purge all remaining events having to do with this window
		WindowEventList::iterator anItr = mWindowEventList.begin();
		while(anItr!=mWindowEventList.end())
		{
			WindowEventBase *anEvent = *anItr;
			if(anEvent->mWindow==theWindow)
				mWindowEventList.erase(anItr++);
			else
				++anItr;
		}

		if(theWindow==mModalWindow)
			EndDialog(-1);

		if(theWindow==mPopupWindowSave)
			mPopupWindowSave = NULL;

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SetModal(Component *theComponent, bool isModal)
{
	theComponent->SetComponentFlags(ComponentFlag_AllowModalInput,isModal);
	Container *aParent = theComponent->GetParent();
	while(aParent!=NULL)
	{
		aParent->SetComponentFlags(ComponentFlag_AllowModalInput,isModal);
		if(isModal)
			aParent->EnterModal();
		else
			aParent->LeaveModal();

		aParent = aParent->GetParent();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::CalcModality()
{
	// Clear old modality
	if(mPopupInputComponent.get()!=NULL)
		SetModal(mPopupInputComponent,false);
	if(mModalComponent!=NULL)
		SetModal(mModalComponent,false);

	bool modal = false;
	mPopupIsUp = false;
	if(!mPopupList.empty())
	{
		// Popup always is allowed to have input
		mPopupIsUp = true;
		PopupList::iterator anItr = mPopupList.begin();
		while(anItr!=mPopupList.end())
		{
			Component *aPopup = *anItr;
			aPopup->SetComponentFlags(ComponentFlag_AllowModalInput | ComponentFlag_IsPopup,true);
			++anItr;
		}

		if(PopupFlagSet(PopupFlag_IsModal))
			modal = true;
	}
	
	if(mModalComponent!=NULL)
	{
		if(!modal) // If not in a modal popup then allow the dialog to get the input
			SetModal(mModalComponent,true);

		modal = true;
	}

	if(!mPopupIsUp && mPopupOwnerComponent.get()!=NULL)
		mPopupOwnerComponent = NULL;

	if(mPopupInputComponent.get()!=NULL)
	{ 
		if(mPopupIsUp)
			SetModal(mPopupInputComponent,true);
		else
			mPopupInputComponent = NULL;
	}

	WindowSet::iterator anItr = mWindowSet.begin();
	while(anItr!=mWindowSet.end())
	{
		Window *aWindow = *anItr;
		if(aWindow==mModalWindow && !mPopupIsUp && mModalComponent==aWindow->GetRoot())
			aWindow->SetModal(false);
		else
			(*anItr)->SetModal(modal);
		
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WindowManager::NotModalWindow(Window *theWindow)
{
	if(mModalWindow!=NULL && theWindow!=mModalWindow)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WindowManager::CheckModalValid()
{
	if(mModalWindow!=NULL)
	{
		if(!mModalWindow->IsOpen())
			return false;

		if(!mModalWindow->IsShowing())
			return false;

		if(mModalComponent!=NULL)
		{
			if(!mModalComponent->IsVisible())
				return false;

			if(mModalComponent->GetRoot()!=mModalWindow->GetRoot())
				return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WindowManager::DoDialog(Window *theWindow, Component *theComponent)
{
	ComponentPtr anOldDialog = mModalComponent;
	WindowPtr anOldModalWindow = mModalWindow;
	mDialogResult = 0;

	if(anOldDialog.get()!=NULL)
		anOldDialog->SetComponentFlags(ComponentFlag_AllowModalInput,false);

	if(theComponent==NULL)
		theComponent = theWindow->GetRoot();

	if(mModalComponent!=NULL)
		SetModal(mModalComponent,false);

	mModalWindow = theWindow;
	mModalComponent = theComponent;
	CalcModality();
	mModalWindow->SetFocus();
	theComponent->RequestFocus();

//	if(CheckModalValid())
		MessageLoop();

	SetModal(mModalComponent,false);
	mModalWindow = anOldModalWindow;
	mModalComponent = anOldDialog;
	CalcModality();

//	if(mModalWindow!=NULL)
//		mModalWindow->SetFocus();

	return mDialogResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::EndDialog(int theResult, Window *theWindow, Component *theDialog)
{
	if(theWindow!=NULL && theWindow!=mModalWindow)
		return;

	if(theDialog!=NULL && theDialog!=mModalComponent)
		return;

	mDialogResult = theResult;
	mTargetMsgLoopDepth--;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::ExitMessageLoop()
{
	mTargetMsgLoopDepth--;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::AdjustPopupPosition(Window *theParent, Component *theComponent, bool inOwnWindow, const WONRectangle &theAvoidRect)
{
	WONRectangle aBoundingRect;
	WONRectangle anAvoidRect = theAvoidRect;
	if(inOwnWindow)
	{
		GetScreenRect(aBoundingRect);

		WONRectangle aWindowRect;
		theParent->GetScreenPos(aWindowRect, true);


//		int aNewX = theComponent->Left() + aWindowRect.Left();
//		int aNewY = theComponent->Top() + aWindowRect.Top();
//		theComponent->SetPos(aNewX,aNewY);

//		anAvoidRect.x -= aWindowRect.Left();
//		anAvoidRect.y -= aWindowRect.Top();	
		aBoundingRect.x -= aWindowRect.Left();
		aBoundingRect.y -= aWindowRect.Top();
	}
	else
	{
		RootContainer* aRoot = theParent->GetRoot();
		aBoundingRect = aRoot->GetBounds();
	}

	int dx = 0, dy = 0;
	if(theComponent->Right() > aBoundingRect.Right()) // need to move left
	{
		dx = aBoundingRect.Right() - theComponent->Right();
		theComponent->SetPos(theComponent->Left() + dx, theComponent->Top());
		if(anAvoidRect.Width()>0 && theComponent->Left()<=theAvoidRect.Right() && theComponent->Right()>=theAvoidRect.Left())
			theComponent->SetPos(anAvoidRect.Left()-theComponent->Width(),theComponent->Top());
	}
	else if(theComponent->Left() < aBoundingRect.Left()) // need to move right
	{
		dx = aBoundingRect.Left() - theComponent->Left();
		theComponent->SetPos(theComponent->Left() + dx,theComponent->Top());
		if(anAvoidRect.Width()>0 && theComponent->Left()<=theAvoidRect.Right() && theComponent->Right()>=theAvoidRect.Left())
			theComponent->SetPos(anAvoidRect.Right(),theComponent->Top());
	}

	if(theComponent->Bottom() > aBoundingRect.Bottom()) // need to move up
	{
		dy = aBoundingRect.Bottom() - theComponent->Bottom();
		theComponent->SetPos(theComponent->Left(), theComponent->Top() + dy);
		if(anAvoidRect.Height()>0 && theComponent->Top()<=theAvoidRect.Bottom() && theComponent->Bottom()>=theAvoidRect.Top())
			theComponent->SetPos(theComponent->Left(), anAvoidRect.Top()-theComponent->Height());
	}
	else if(theComponent->Top() < aBoundingRect.Top()) // need to move down
	{
		dy = aBoundingRect.Top() - theComponent->Top();
		theComponent->SetPos(theComponent->Left(), theComponent->Top() + dy);
		if(anAvoidRect.Height()>0 && theComponent->Top()<=theAvoidRect.Bottom() && theComponent->Bottom()>=theAvoidRect.Top())
			theComponent->SetPos(theComponent->Left(), anAvoidRect.Bottom());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::DoPopup(const PopupParams &theParams)
{
	// Make sure these guys get reference counted
	WindowPtr aParent = theParams.mParent;
	ComponentPtr aPopup = theParams.mPopup;

	if(mInDoPopup || mInEndPopup)
		return;

	bool additional = (theParams.mFlags&PopupFlag_Additional) && !mPopupList.empty();
	Window *aFocusCheckWindow = aParent;
	if(additional)
		aFocusCheckWindow = mPopupOwnerWindow;

//	if(!aParent->HasFocus()) // don't allow popup if window doesn't have the input focus
	if(mFocusWindow!=aFocusCheckWindow)
		return;

	mInDoPopup = true;
	if(mPopupIsUp && !additional)
		EndPopup();

	int x = theParams.mx;
	int y = theParams.my;
	if(x==-1)
		x = aParent->GetMouseX();
	if(y==-1)
		y = aParent->GetMouseY();

	aPopup->SetPos(x,y);

	mEndPopup = false;

	if(!additional)
	{
		mPopupFlags = theParams.mFlags;
		mPopupOwnerWindow = aParent;
		mPopupInputComponent = theParams.mInputComponent;
	}

	mPopupList.push_back(aPopup);
	bool inOwnWindow = mPopupInOwnWindowDef;
	if(PopupFlagSet(PopupFlag_InOwnWindow))
		inOwnWindow = true;
	else if(PopupFlagSet(PopupFlag_NotInOwnWindow))
		inOwnWindow = false;

	if(PopupFlagSet(PopupFlag_AdjustPosition))
		AdjustPopupPosition(aParent,aPopup,inOwnWindow,theParams.mAvoidRect);

	WindowPtr aPopupWindow;
	if(inOwnWindow)
	{
		WONRectangle aRect;
		aParent->GetScreenPos(aRect,true);
		aRect.x+=aPopup->Left();
		aRect.y+=aPopup->Top();
		aRect.width = aPopup->Width();
		aRect.height = aPopup->Height();

		if(mPopupWindowSave.get()!=NULL)
		{
			if(mPopupWindowSave->mParent!=aParent)
				mPopupWindowSave->Close();
			else
			{
				mPopupWindowSave->Move(aRect.x, aRect.y, aRect.width, aRect.height);
				mPopupWindowSave->mIsPopup = true;
				mPopupWindowSave->Show(true);
				aPopupWindow = mPopupWindowSave;
			}
			mPopupWindowSave = NULL;
		}
				
		if(aPopupWindow.get()==NULL)
		{
			aPopupWindow = new PlatformWindow;
			aPopupWindow->mIsOneComponentWindow = true;
			aPopupWindow->SetCreateFlags(CreateWindow_Popup);
			aPopupWindow->SetParent(aParent);
			aPopupWindow->mIsPopup = true;

			aPopupWindow->Create(aRect);
		}
		aPopupWindow->mIsPopup = true;

		if(PopupFlagSet(PopupFlag_WantInput))
		{
			mPopupOwnerWindow->SetFocus();	
			mPopupOwnerWindow->GetRoot()->LostFocus();
		}

		aPopup->SetPos(0,0);
	}
	else
	{
		aPopupWindow = aParent;
		aPopup->SetComponentFlags(ComponentFlag_GrabBG,true);
		if(PopupFlagSet(PopupFlag_DontWantMouse))
			aPopup->SetComponentFlags(ComponentFlag_NoInput,true);
	}

	mPopupOwnerComponent = theParams.mOwnerComponent;
	if(theParams.mOwnerComponent!=NULL && aPopup->GetOwnColorScheme()==NULL)
		aPopup->SetColorScheme(theParams.mOwnerComponent->GetColorScheme());

	aPopup->SetComponentFlags(ComponentFlag_AlwaysOnTop,true);
	aPopupWindow->AddComponent(aPopup);
	if(PopupFlagSet(PopupFlag_WantInput))
		aPopup->RequestFocus();

	if(mDoMouseCapturePopupSupport)
		mPopupOwnerWindow->SetMouseCapture(true);

	mInDoPopup = false;
	CalcModality();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::EndPopup(Component *thePopup)
{
	if(mPopupList.empty())
		return;

	if(thePopup!=NULL && !thePopup->ComponentFlagSet(ComponentFlag_IsPopup))
		return;

	bool inEndPopup = mInEndPopup;
	mInEndPopup = true; // protect adding popups while removing popups
	while(!mPopupList.empty() && mPopupList.back()!=thePopup)
		EndPopup(mPopupList.back());

	if(mPopupList.empty())
	{
		mInEndPopup = inEndPopup;
		return;
	}

	mEndPopup = false;

	if(mPopupIsUp)
	{
		ComponentPtr aPopupComponent = mPopupList.back();
		WindowPtr aPopupWindow = aPopupComponent->GetWindow();
		WindowPtr aPopupOwnerWindow = mPopupOwnerWindow;
		mPopupList.pop_back();

		if(mPopupFlags&PopupFlag_FireEventOnEnd)
			aPopupComponent->FireEvent(ComponentEvent_EndPopup);

		if(mPopupList.empty())
		{
			if(mDoMouseCapturePopupSupport)
				mPopupOwnerWindow->SetMouseCapture(false);

			mPopupOwnerWindow = NULL;
			mMouseDownWindow = NULL;
		}

		if(aPopupWindow.get()!=aPopupOwnerWindow.get())
		{
			aPopupWindow->mIsPopup = false; // Mark as not Popup so we don't accidentally end all popups in InputEvent_WindowClose
			if(mPopupList.empty())
				mPopupWindowSave = aPopupWindow;

			aPopupWindow->GetRoot()->RemoveAllChildren();
			if(mPopupWindowSave.get()!=NULL)
			{
				aPopupWindow->GetRoot()->RemoveAllChildren();
				if(!mInDoPopup)
					aPopupWindow->Show(false);
			}
			else
				aPopupWindow->Close();
			
			if(mPopupList.empty() && PopupFlagSet(PopupFlag_WantInput))
			{
				if(mFocusWindow==aPopupOwnerWindow)
					aPopupOwnerWindow->GetRoot()->GotFocus();
			}
		}
		else
		{
			aPopupComponent->SetComponentFlags(ComponentFlag_AllowModalInput | ComponentFlag_IsPopup,false);
			aPopupWindow->GetRoot()->RemoveChild(aPopupComponent);
		}

		CalcModality();
	}

	mInEndPopup = inEndPopup;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::CheckEndPopup(Component *theAncestor)
{
	if(!mPopupIsUp || mPopupOwnerComponent.get()==NULL)
		return;

	Component *aComponent = mPopupOwnerComponent;
	while(aComponent!=NULL)
	{
		if(aComponent==theAncestor)
		{
			EndPopup();
			return;
		}
		aComponent = aComponent->GetParent();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::GetNextEvent()
{
	Sleep(20);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WindowManager::MessageLoop()
{
	if(mWindowSet.empty())
		return -1;

	int oldPauseEvents = mPauseEvents;
	mPauseEvents = 0;

	CreateRef();

	mTargetMsgLoopDepth =  ++mCurMsgLoopDepth;
	while(mCurMsgLoopDepth<=mTargetMsgLoopDepth)
	{
		GetNextEvent();
//		ProcessEvents();
	}

	mPauseEvents = oldPauseEvents;

	mCurMsgLoopDepth--;
	int aDialogResult = mDialogResult;
	Release();

	return aDialogResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::PumpOnce()
{
	GetNextEvent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::PostEvent(WindowEventBase *theEvent, bool needSignal)
{
	WONAPI::AutoCrit aCrit(mDataCrit);
	mWindowEventList.push_back(theEvent);
	if(mCurMsgLoopDepth<=0)
		needSignal = true;

	aCrit.Leave();

	if(needSignal)
		Signal();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::RemoveAllTimerEvents()
{
	if(mCallingTimers)
		mRemoveTimerEvents = mTimerEvents;
	else
		mTimerEvents.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::AddTimerEvent(WindowEventBase *theEvent)
{
	mTimerEvents.insert(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::RemoveTimerEvent(WindowEventBase *theEvent)
{
	if(mCallingTimers)
		mRemoveTimerEvents.insert(theEvent);
	else
		mTimerEvents.erase(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::FindMouseWindow(MouseEvent *theEvent)
{
	if(!mPopupIsUp)
		return;

	if(PopupFlagSet(PopupFlag_DontWantMouse) && theEvent->mWindow->mIsPopup)
	{
		WONRectangle aPopupRect, anOwnerRect;
		theEvent->mWindow->GetScreenPos(aPopupRect,true);
		mPopupOwnerWindow->GetScreenPos(anOwnerRect,true);
		anOwnerRect.x -= aPopupRect.x;
		anOwnerRect.y -= aPopupRect.y;
		if(anOwnerRect.Contains(theEvent->mx, theEvent->my))
		{
			theEvent->mWindow = mPopupOwnerWindow;
			theEvent->mx -= anOwnerRect.x;
			theEvent->my -= anOwnerRect.y;
			return;
		}
	}

	if(!mDoMouseCapturePopupSupport)
		return;

	WONRectangle anOwnerRect;
	theEvent->mWindow->GetScreenPos(anOwnerRect,true);
	if(mMouseDownWindow!=NULL && mMouseDownWindow->mMouseIsDown[MouseButton_Left])
	{
		WONRectangle aRect;
		mMouseDownWindow->GetScreenPos(aRect,true);
		aRect.x -= anOwnerRect.x;
		aRect.y -= anOwnerRect.y;

		theEvent->mWindow = mMouseDownWindow;
		theEvent->mx -= aRect.x;
		theEvent->my -= aRect.y;
		return;
	}

	PopupList::iterator anItr  = mPopupList.end();
	do
	{
		WONRectangle aRect;

		--anItr;
		Window *aWindow = (*anItr)->GetWindow();
		aWindow->GetScreenPos(aRect,true);
		aRect.x -= anOwnerRect.x;
		aRect.y -= anOwnerRect.y;

		if(aRect.Contains(theEvent->mx, theEvent->my))
		{
			theEvent->mWindow = aWindow;
			theEvent->mx -= aRect.x;
			theEvent->my -= aRect.y;
			return;
		}
	} while(anItr!=mPopupList.begin());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WindowManager::CheckPopupClick(MouseEvent *theEvent)
{
	FindMouseWindow(theEvent);
	if(theEvent->mButtonOrAmount==MouseButton_Left)
		mMouseDownWindow = theEvent->mWindow;

	if(PopupFlagSet(1<<theEvent->mButtonOrAmount)) // End Popup because of click?
	{
		int x = theEvent->mx, y = theEvent->my;
		Container *aContainer = theEvent->mWindow->GetRoot();
		bool endPopup = true;
		while(true)
		{
			x-=aContainer->Left();
			y-=aContainer->Top();

			Component *aClickChild = aContainer->WMFindChildAt(x, y);
			if(aClickChild!=NULL)
			{ 
				if(aClickChild==mPopupInputComponent.get() || aClickChild->ComponentFlagSet(ComponentFlag_IsPopup))
				{
					endPopup = false;
					break;
				}
				else if(mPopupInputComponent.get()==NULL)
					break;
				else
				{
					aContainer = dynamic_cast<Container*>(aClickChild);
					if(aContainer==NULL)
						break;
				}
			}
			else
				break;
		}

		if(endPopup)
		{
			bool eat = PopupFlagSet(PopupFlag_EatMouseDownOnEnd);
			EndPopup();
			if(eat)
				return true;
		}
	}

	if(theEvent->mWindow->mIsPopup)
		mPopupOwnerWindow->SetFocus();	

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::DeliverEvent(WindowEventBase *theEvent)
{
	switch(theEvent->mType)
	{
		case InputEvent_MouseDoubleClick:
		case InputEvent_MouseDownCheckDoubleClick:
		case InputEvent_MouseDown:
		{
			MouseEvent *anEvent = (MouseEvent*)theEvent;
			int aButton = anEvent->mButtonOrAmount;
			if(mPopupIsUp)
			{
				if(CheckPopupClick(anEvent))
					break;
			}
			if(mFocusWindow!=anEvent->mWindow && !anEvent->mWindow->mIsPopup && !NotModalWindow(anEvent->mWindow))
				anEvent->mWindow->SetFocus();

			if(anEvent->mButtonOrAmount==MouseButton_Left && (!mPopupIsUp || !mDoMouseCapturePopupSupport))
				anEvent->mWindow->SetMouseCapture(true);
		}

		if(theEvent->mWindow->MouseIsInClientArea()) // don't allow mouse down if not in client area
			theEvent->mWindow->HandleMouseEvent((MouseEvent*)theEvent);
		break;

		case InputEvent_MouseMove:
		{
			WindowEventBase *nextEvent = PeekNextEvent();
			if(nextEvent!=NULL && nextEvent->mType==InputEvent_MouseMove && nextEvent->mWindow==theEvent->mWindow)
				return; // condense the events

			FindMouseWindow((MouseEvent*)theEvent);
			if(mPopupIsUp && theEvent->mType==InputEvent_MouseMove && PopupFlagSet(PopupFlag_EndOnMouseMove))
				EndPopup();

			theEvent->mWindow->HandleMouseEvent((MouseEvent*)theEvent);
			break;
		}

		case InputEvent_MouseUp:
		{
			MouseEvent *anEvent = (MouseEvent*)theEvent;
			FindMouseWindow(anEvent);
			if(anEvent->mButtonOrAmount==MouseButton_Left)
				mMouseDownWindow = NULL;

			if(anEvent->mButtonOrAmount==MouseButton_Left && (!mPopupIsUp || !mDoMouseCapturePopupSupport))
				theEvent->mWindow->SetMouseCapture(false);

			theEvent->mWindow->HandleMouseEvent((MouseEvent*)theEvent);
			break;
		}

		case InputEvent_MouseWheel:
		{
			MouseEvent *anEvent = (MouseEvent*)theEvent;
			FindMouseWindow(anEvent);
			if(mPopupIsUp && !PopupFlagSet(PopupFlag_DontWantMouse))
			{
				// Always send the mousewheel to the popup unless it doesn't want mouse input
				Component *aPopupComponent = mPopupList.back();
				aPopupComponent->MouseWheel(anEvent->mButtonOrAmount);
				return;

/*				Window *aPopupWindow = mPopupList.back()->GetWindow();
				if(aPopupWindow!=theEvent->mWindow)
				{
					WONRectangle popupOwnerWindow, popupWindow;
					anEvent->mWindow->GetScreenPos(popupOwnerWindow,true);
					aPopupWindow->GetScreenPos(popupWindow,true);
					int x = anEvent->mx + popupOwnerWindow.Left();
					int y = anEvent->my + popupOwnerWindow.Top();
					if(popupWindow.Contains(x,y) || PopupFlagSet(PopupFlag_WantInput))
					{
						anEvent->mWindow = aPopupWindow;
						anEvent->mx += popupOwnerWindow.Left() - popupWindow.Left();
						anEvent->my += popupOwnerWindow.Top() - popupWindow.Top();
					}
				}*/
			}
			theEvent->mWindow->HandleMouseEvent(anEvent);
			break;
		}

		case InputEvent_KeyDown:
		case InputEvent_KeyUp:
		case InputEvent_KeyChar:
			if(mPopupIsUp)
			{
				int aKey = ((KeyEvent*)theEvent)->mKey;
				if(theEvent->mType==InputEvent_KeyDown)
				{	
					if(PopupFlagSet(PopupFlag_EndOnKeyDown) || PopupFlagSet(PopupFlag_EndOnEscape) && aKey==KEYCODE_ESCAPE)
					{
						EndPopup();
						break;
					}
				}
				
				if(PopupFlagSet(PopupFlag_WantInput))
				{
					Window *aPopupWindow = mPopupList.back()->GetWindow();
					if(aPopupWindow!=theEvent->mWindow)
						theEvent->mWindow = aPopupWindow;
				}
			}
			theEvent->mWindow->HandleKeyEvent((KeyEvent*)theEvent);
			break;

		case InputEvent_GotFocus:
			if(mIsFullScreen)
				SetScreenResolution(mFullScreenWidth,mFullScreenHeight);

			NotifyGotFocus(theEvent->mWindow);
			theEvent->mWindow->HandleInputEvent((InputEvent*)theEvent);
			if(NotModalWindow(theEvent->mWindow))
				mModalWindow->SetFocus();
			return;

		case InputEvent_LostFocus:
			NotifyLostFocus();
			theEvent->mWindow->HandleInputEvent((InputEvent*)theEvent);
			return;

		case InputEvent_SizeChange:
			if(theEvent->mWindow==mPopupOwnerWindow)
				EndPopup();			
			else
			{
				WindowEventBase *nextEvent = PeekNextEvent();
				if(nextEvent!=NULL && nextEvent->mType==InputEvent_SizeChange && nextEvent->mWindow==theEvent->mWindow)
				{
					SizeEvent *s1 = (SizeEvent*)theEvent;
					SizeEvent *s2 = (SizeEvent*)nextEvent;
					if(s2->mx==-1)
						s2->mx = s1->mx;
					if(s2->my==-1)
						s2->my = s1->my;
					if(s2->mWidth==-1)
						s2->mWidth = s1->mWidth;
					if(s2->mHeight==-1)
						s2->mHeight = s1->mHeight;
					return; // condense the events
				}
			}

			theEvent->mWindow->HandleInputEvent((InputEvent*)theEvent);
			return;


		case InputEvent_MouseExit:
			theEvent->mWindow->HandleInputEvent((InputEvent*)theEvent);
			return;

		case InputEvent_WindowClose:
		{
			if(mPopupIsUp)
				if(theEvent->mWindow==mPopupOwnerWindow || theEvent->mWindow->mIsPopup)
					EndPopup();

//			if(theEvent->mWindow==mModalWindow) // this is not quite sufficient
//				EndDialog();

			if(mFocusWindow==theEvent->mWindow)
				NotifyLostFocus();

			if(mMouseDownWindow==theEvent->mWindow)
				mMouseDownWindow = NULL;

			theEvent->mWindow->ClosePrv();
			WindowEventBasePtr aCloseEvent = theEvent->mWindow->mCloseEvent;
			if(aCloseEvent.get()!=NULL)
			{
				aCloseEvent->mWindow = theEvent->mWindow;
				aCloseEvent->Deliver();
			}

			RemoveWindow(theEvent->mWindow);
		}
		return;


		default:
			theEvent->Deliver();
			return;
	}

	mLastUserInputTick = GetTickCount();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::SubscribeToBroadcast(Component *theComponent, bool subscribe)
{
	if(subscribe)
		mBroadcastSet.insert(theComponent);
	else
		mBroadcastSet.erase(theComponent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::ClearSubscriptions()
{
	mBroadcastSet.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::BroadcastEvent(ComponentEvent *theEvent)
{
	BroadcastSet::iterator anItr = mBroadcastSet.begin();
	while(anItr!=mBroadcastSet.end())
	{
		Component *aComponent = *anItr;
		aComponent->HandleComponentEvent(theEvent);
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WindowEventBase* WindowManager::PeekNextEvent()
{
	AutoCrit aCrit(mDataCrit);
	if(mWindowEventList.empty())
		return NULL;
	else
		return  mWindowEventList.front();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::ProcessEvents()
{
	if(mPauseEvents)
		return;

	DWORD aTick = GetTickCount();
	DWORD anElapsed = aTick - mLastTimerTick;
	if(anElapsed >= 50)
	{
		if(anElapsed >= 1000)
			anElapsed = 1000;

		mLastTimerTick = aTick;

		if(mPopupIsUp && mFocusWindow!=mPopupOwnerWindow && PopupFlagSet(PopupFlag_EndOnFocusChange))
		{
			if(mFocusWindow!=NULL && mFocusWindow->mIsPopup)
			{
				mEndPopup = false;
				mPopupOwnerWindow->SetFocus();
			}
			else if(mEndPopup)
				EndPopup();
			else
				mEndPopup = true;
		}
		else
			mEndPopup = false;

		if(mLastLoseFocusTick!=0 && mFocusWindow==NULL && aTick-mLastLoseFocusTick>50)
		{
			mLastLoseFocusTick = 0;
			if(mIsFullScreen)
				RestoreScreenResolution();
		}	

		WindowSet::iterator anItr = mWindowSet.begin();
		while(anItr!=mWindowSet.end())
		{
			Window *aWindow = *anItr;
			aWindow->TimerHook(anElapsed);
			++anItr;
		}

		if(!mTimerEvents.empty())
		{
			mCallingTimers = true;
			for(WindowEventSet::iterator aTimerItr = mTimerEvents.begin(); aTimerItr != mTimerEvents.end(); ++aTimerItr)
				(*aTimerItr)->Deliver();
			mCallingTimers = false;
		}

		if(!mRemoveTimerEvents.empty()) // erase any timers that were removed while delivering timer events
		{
			for(WindowEventSet::iterator aTimerItr = mRemoveTimerEvents.begin(); aTimerItr != mRemoveTimerEvents.end(); ++aTimerItr)
				mTimerEvents.erase(*aTimerItr);
		}
	}

	WONAPI::AutoCrit aCrit(mDataCrit, false);
	while(!mWindowEventList.empty()  && mCurMsgLoopDepth<=mTargetMsgLoopDepth)
	{
		aCrit.Enter();
		WindowEventBasePtr anEvent = mWindowEventList.front();
		mWindowEventList.pop_front();
		aCrit.Leave();

		DeliverEvent(anEvent);
	}


	WindowSet::iterator anItr = mWindowSet.begin();
	while(anItr!=mWindowSet.end())
	{
		Window *aWindow = *anItr;
		aWindow->MessageLoopHook();
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::PauseEvents()
{
	mPauseEvents++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WindowManager::UnPauseEvents()
{
	mPauseEvents--;

	if(mPauseEvents < 0)
		mPauseEvents = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AutoWMPauseEvents::AutoWMPauseEvents()
{
	mPauseCount = 0;
	Pause();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AutoWMPauseEvents::~AutoWMPauseEvents()
{
	UnPause();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AutoWMPauseEvents::Pause()
{
	WindowManager *wm = WindowManager::GetDefaultWindowManager();
	if(wm!=NULL)
	{
		mPauseCount++;
		wm->PauseEvents();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AutoWMPauseEvents::UnPause()
{
	if(mPauseCount<=0)
		return;

	WindowManager *wm = WindowManager::GetDefaultWindowManager();
	if(wm!=NULL)
	{
		mPauseCount--;
		wm->UnPauseEvents();
	}
}
