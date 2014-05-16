#include "XWindow.h"
#include "XFont.h"
#include "XCursor.h"
#include "XNativeImage.h"
#include "RawImage.h"
#include "WindowManager.h"
#include <signal.h>
#include <sys/poll.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

static WONAPI::XCursorPtr gBlankCursor;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::XWindowManager::XWindowManager(Display *theDisplay)
{
	mDoMouseCapturePopupSupport = true;

	if(theDisplay==NULL)
	{
		theDisplay = XOpenDisplay("");
		if(theDisplay == NULL)
		{
			mIsValid = false;
			printf("Cannot connect to xserver.\n");
			return;
		}

	}

	mDisplay = theDisplay;
	WONAPI::Cursor::SetStandardCursor(StandardCursor_Arrow, new XCursor(XCreateFontCursor(theDisplay,XC_left_ptr));
	WONAPI::Cursor::SetStandardCursor(StandardCursor_IBeam, new XCursor(XCreateFontCursor(theDisplay,XC_xterm));
	WONAPI::Cursor::SetStandardCursor(StandardCursor_Hand, new XCursor(XCreateFontCursor(theDisplay,XC_hand2));
	WONAPI::Cursor::SetStandardCursor(StandardCursor_EastWest, new XCursor(XCreateFontCursor(theDisplay,XC_sb_h_double_arrow));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindowManager::sig_alarm_handler(int)
{
	XWindowManager *aManager = (XWindowManager*)WONAPI::WindowManager::GetDefaultWindowManager();
	if(!aManager->mProcessingEvent)
		aManager->ProcessEvents();

	int aDelay = 50;
    int aDelaySec = aDelay/1000;
    int aDelayUSec = (aDelay%1000)*1000;

    itimerval aTimer = { {0,0}, {aDelaySec, aDelayUSec} };
    setitimer(ITIMER_REAL, &aTimer, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::XWindowManager::AddWindow(Window *theWindow)
{
	if(!WindowManager::AddWindow(theWindow))
		return false;

	mWindowMap[((XWindow*)theWindow)->mWindow] = (XWindow*)theWindow;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::XWindowManager::RemoveWindow(Window *theWindow)
{
	if(!WindowManager::RemoveWindow(theWindow))
		return false;

	mWindowMap.erase(((XWindow*)theWindow)->mWindow);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindowManager::GetScreenRect(WONRectangle &theRect)
{
	theRect.x = theRect.y = 0;
	theRect.width = DisplayWidth(mDisplay, DefaultScreen(mDisplay));
	theRect.height = DisplayHeight(mDisplay, DefaultScreen(mDisplay));
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindowManager::GetNextEvent()
{
	mProcessingEvent = false;

	static bool haveSetSignal = false;
	static pollfd aPoll;
	if(!haveSetSignal)
	{
		haveSetSignal = true;
//		signal(SIGALRM,sig_alarm_handler);
//		sig_alarm_handler(0);

		aPoll.fd = XConnectionNumber(mDisplay);
		aPoll.events = POLLIN; 
	}

	if(!XPending(mDisplay))
	{
/*		int s = XConnectionNumber(mDisplay);
		fd_set aSet;
		FD_ZERO(&aSet);
		FD_SET(s, &aSet);
		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 50000;
		select(s, &aSet, NULL, NULL, &tv);*/
		if(poll(&aPoll,1,50)<1)
		{
			ProcessEvents();
			return;
		}

	}

//	if(XPending(mDisplay)>0)
//	{
		XEvent xevent; 
		XNextEvent(mDisplay, &xevent);
		mProcessingEvent = true;
		::Window aWindow = xevent.xany.window;
		WindowMap::iterator anItr = mWindowMap.find(aWindow);
		if(anItr!=mWindowMap.end())
		{
			XWindow *aWindow = anItr->second;
			if(aWindow->WindowProc(xevent) && XPending(mDisplay)<=0)
				ProcessEvents();
		}
		mProcessingEvent = false;
//	}
//	else
//		((XWindow*)mDefaultWindow)->MessageLoopHook();

//	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
void WONAPI::XWindow::sig_alarm_handler(int)
{
	if(!mProcessingEvent)
	{
		WindowMap::iterator anItr = mWindowMap.begin();
		while(anItr!=mWindowMap.end())
		{
			XWindow *aWindow = anItr->second;
			if(!aWindow->mOpen)
				mWindowMap.erase(anItr++);
			else
			{
				aWindow->MessageLoopHook();
				++anItr;
			}
		}
	}

	int aDelay = 50;
    int aDelaySec = aDelay/1000;
    int aDelayUSec = (aDelay%1000)*1000;

    itimerval aTimer = { {0,0}, {aDelaySec, aDelayUSec} };
    setitimer(ITIMER_REAL, &aTimer, NULL);
}*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::XWindow::XWindow()
{
	mIsMapped = false;
	mWindow = 0;
	mResizeDelay = 0;
	mDisplay = ((XWindowManager*)mWindowManager)->GetDisplay();
	mOldX = mOldY = mOldWidth = mOldHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::XWindow::~XWindow()
{
	int x = 0;
	x++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::FontPtr WONAPI::XWindow::GetNativeFont(const FontDescriptor &theDescriptor)
{
	return new WONAPI::XFont(mDisplayContext, theDescriptor);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::Create(const WONRectangle& r)
{
	mIsMapped = false;

	mDisplayContext = new DisplayContext;
	mDisplayContext->mDisplay = mDisplay;
	mDisplayContext->mScreen = DefaultScreen(mDisplay);
	mDisplayContext->mVisual = DefaultVisual(mDisplay, mDisplayContext->mScreen);
	mDisplayContext->mDepth = DefaultDepth(mDisplay, mDisplayContext->mScreen);
	mDisplayContext->mColormap = DefaultColormap(mDisplay, mDisplayContext->mScreen);
	mDisplayContext->mWhitePixel = WhitePixel(mDisplay, mDisplayContext->mScreen);
	mDisplayContext->mBlackPixel = BlackPixel(mDisplay, mDisplayContext->mScreen);	
	DisplayContext::StaticInit(mDisplayContext);
	int aWidth = r.width;
	int aHeight = r.height;
	if(aWidth<=0) 
		aWidth = 1;
	if(aHeight<=0)
		aHeight = 1;

	mWindow = XCreateSimpleWindow(mDisplay, DefaultRootWindow(mDisplay), r.x, r.y, aWidth, aHeight, 0, mDisplayContext->mWhitePixel, mDisplayContext->mBlackPixel);
	if(!mTitle.empty())
		SetTitle(mTitle);

	Window::Create(r);

	if(mCreateFlags & CreateWindow_Popup)
	{
		XSetWindowAttributes attributes;
		attributes.override_redirect = TRUE;
		XChangeWindowAttributes( mDisplay, mWindow, CWOverrideRedirect, &attributes);
	}

	mDisplayContext->mDrawable = mWindow;
	mDisplayContext->CreateNewGC();
	mOffscreenImage = CreateImage(aWidth,aHeight);

	if(gBlankCursor.get()==NULL)
	{
		WONAPI::RawImage1Ptr aBlankImage = new WONAPI::RawImage1(12, 12);
		XNativeMaskPtr aMask = (XNativeMask*)aBlankImage->GetMask(mDisplayContext).get();
		Pixmap aPixmap = aMask->GetMask();
		XColor foreground;
		foreground.pixel = mDisplayContext->mWhitePixel;
		XColor background;
		background.pixel = mDisplayContext->mBlackPixel;
		gBlankCursor = new XCursor(
			XCreatePixmapCursor(mDisplay, aPixmap, aPixmap, &foreground, &background, 0, 0)
		);
	}
	
/*	int aMinKeyCodes = 0;
	int aMaxKeyCodes = 0;
	XDisplayKeycodes(mDisplay, &aMinKeyCodes, &aMaxKeyCodes);
	mKeyMap = XGetKeyboardMapping(mDisplay, 
*/
	XSelectInput(mDisplay, mWindow, 
		PointerMotionMask | ButtonReleaseMask |  ButtonPressMask | 
		KeyPressMask | KeyReleaseMask | ExposureMask | LeaveWindowMask |
		FocusChangeMask | StructureNotifyMask);

	if(!(mCreateFlags&CreateWindow_HideAtFirst))
		XMapRaised(mDisplay, mWindow); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::ClosePrv()
{
	Window::ClosePrv();
	if(mWindow!=0)
	{
//		mWindowMap.erase(mWindow);
		XDestroyWindow(mDisplay, mWindow);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::SetTitleHook()
{
	std::string aTitle = mTitle;
	XStoreName(mDisplay,mWindow,(char*)aTitle.c_str());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::GetScreenPos(WONRectangle &theRectR, bool clientArea)
{
	XWindowAttributes attributes;
	XGetWindowAttributes(mDisplay, mWindow, &attributes);
	theRectR.x = attributes.x;
	theRectR.y = attributes.y;
	theRectR.width = attributes.width;
	theRectR.height = attributes.height;
	::Window aChild;

//	if(clientArea)
		attributes.x = attributes.y = 0;
	
	XTranslateCoordinates(mDisplay, mWindow, attributes.root, attributes.x, attributes.y, &theRectR.x, &theRectR.y, &aChild);                         
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::Move(int x, int y, int width, int height)
{
	if(width==-1 || height==-1)
	{
		WONRectangle aRect;
		GetScreenPos(aRect);
		if(width==-1)
			width = aRect.Width();

		if(height==-1)
			height = aRect.Height();
	}

	if(width<=0)
		width = 1;

	if(height<=0)
		height = 1;
	XMoveResizeWindow(mDisplay, mWindow, x, y, width, height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::SetFocus()
{
	if(mIsMapped)
		XSetInputFocus(mDisplay, mWindow, RevertToNone, CurrentTime);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::ShowWindowHook(bool show)
{
	if(show)
	{
		XMapWindow(mDisplay, mWindow);
		XRaiseWindow(mDisplay, mWindow);
	}
	else
		XUnmapWindow(mDisplay, mWindow);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::SetMouseCapture(bool on)
{
	if(on)
	{
	    XGrabPointer(mDisplay, mWindow, TRUE,
						ButtonPressMask | ButtonReleaseMask |
						ButtonMotionMask | EnterWindowMask |
						LeaveWindowMask | PointerMotionMask,
						GrabModeAsync, GrabModeAsync,
						None, None, CurrentTime );
	}
	else
		XUngrabPointer(mDisplay, CurrentTime);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::SetNativeCursor(Cursor *theCursor)
{
	if(mIsPopup && mParent!=NULL)
		((XWindow*)mParent)->SetNativeCursor(theCursor);
	else
		XDefineCursor(mDisplay, mWindow, ((XCursor*)theCursor)->mNativeCursor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::XWindow::ShowNativeCursorHook(bool show)
{
	if(mIsPopup && mParent!=NULL)
		((XWindow*)mParent)->ShowNativeCursorHook(show);
	else if(!show)
		XDefineCursor(mDisplay, mWindow, gBlankCursor->mNativeCursor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONAPI::XWindow::MessageLoopHook()
{
	if(!Window::MessageLoopHook())
		return false;

	if(mRootContainer->IsInvalid())
	{
		WONRectangle r = mRootContainer->GetInvalidRect();
		mRootContainer->PrePaint(mOffscreenImage->GetGraphics());
		mRootContainer->Paint(mOffscreenImage->GetGraphics());
		XSetClipMask(mDisplay, mDisplayContext->mGC, None);
		if(r.Width()>0 && r.Height() > 0)
		{
			XRectangle aRect = { r.Left(), r.Top(), r.Width(), r.Height() };
			XSetClipRectangles(mDisplay, mDisplayContext->mGC, 0, 0, &aRect, 1, Unsorted);
		}
		mOffscreenImage->DrawEntire(mDisplayContext,0,0);
		XSetClipMask(mDisplay, mDisplayContext->mGC, None);
		XFlush(mDisplay);
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONAPI::XWindow::WindowProc(XEvent &theEvent)
{	
	switch(theEvent.type)
	{
		case MapNotify: mIsMapped = true; break;
		case UnmapNotify: mIsMapped = false; break;
		
		case ConfigureNotify:
		{
			XConfigureEvent *anEvent = (XConfigureEvent*)&theEvent;
			if(anEvent->x!=mOldX || anEvent->y!=mOldY || anEvent->width!=mOldWidth || anEvent->height!=mOldHeight)
			{
				mOldX = anEvent->x;
				mOldY = anEvent->y;
				mOldWidth = anEvent->width;
				mOldHeight = anEvent->height;
				PostEvent(new SizeEvent(anEvent->x, anEvent->y, anEvent->width, anEvent->height));
			}
		}
		break;


		case Expose:
			mOffscreenImage->DrawEntire(mDisplayContext, 0, 0);
		break;
	
		case MotionNotify: 
		{
			XMotionEvent *anEvent = (XMotionEvent*)&theEvent;
			PostEvent(new MouseEvent(InputEvent_MouseMove,anEvent->x, anEvent->y, anEvent->state));
		}
		break;

		case ButtonPress:
		case ButtonRelease:
		{
			XButtonEvent *anEvent = (XButtonEvent*)&theEvent;
			WindowEventType aType = theEvent.type==ButtonPress?InputEvent_MouseDownCheckDoubleClick:InputEvent_MouseUp;
			PostEvent(new MouseEvent(aType,anEvent->x, anEvent->y, anEvent->state, anEvent->button-1));
		}	
		break;
		
		case KeyPress:
		case KeyRelease:
		{
			XKeyEvent *anEvent = (XKeyEvent*)&theEvent;
			unsigned char aChar = 0;
			KeySym aKeySym = 0;
			XLookupString(anEvent, (char*)&aChar, 1, &aKeySym, NULL);			
		
			if(theEvent.type==KeyPress)
			{	
				if(aChar!=0)
					PostEvent(new KeyEvent(InputEvent_KeyChar, aChar, anEvent->state));

				PostEvent(new KeyEvent(InputEvent_KeyDown, aKeySym, anEvent->state));
			}
			else
			{
				PostEvent(new KeyEvent(InputEvent_KeyUp, aKeySym, anEvent->state));
			}
			
		}	
		break;
	
		case EnterNotify:
		{
			XEnterWindowEvent *anEvent = (XEnterWindowEvent*)&theEvent;
			if(!mMouseIsInClientArea)
				PostEvent(new MouseEvent(InputEvent_MouseMove, anEvent->x, anEvent->y, anEvent->state));
		}
		break;

		case LeaveNotify:
			PostEvent(new InputEvent(InputEvent_MouseExit));
			break;

		case FocusIn:
			PostEvent(new InputEvent(InputEvent_GotFocus));
			break;

		case FocusOut:
			PostEvent(new InputEvent(InputEvent_LostFocus));
			break;

		case DestroyNotify:
			mWindow = 0;
			Close();
			break;
		
		default:
			return 0;
	}

//	mWindowManager->ProcessEvents();
	return 1;
}

	
