#include "MSWindow.h"
#include "MSBMPDecoder.h"
#include "MSFont.h"
#include "MSDisplayContext.h"
#include "MSCursor.h"
#include "WindowManager.h"
#include "MSSound.h"
#include "resource.h"  

using namespace WONAPI;

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static HWND gMSWindowDefaultOwnerHWND = NULL;
MSWindow::DisplayContextAllocator MSWindow::mDisplayContextAllocator = MSWindow::DefDisplayContextAllocator;
MSWindow::FontAllocator MSWindow::mFontAllocator = MSWindow::DefFontAllocator;

void MSWindow::SetDefaultOwnerHWND(HWND theHWND)
{
	gMSWindowDefaultOwnerHWND = theHWND;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const unsigned char gHandCursorData[] = 
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x0c,
	0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x0d,0x80,0x00,0x00,
	0x0d,0xb0,0x00,0x00,0x0d,0xb4,0x00,0x00,0x0d,0xb6,0x00,0x00,0xcf,0xf6,0x00,
	0x00,0xef,0xfe,0x00,0x00,0x6f,0xfe,0x00,0x00,0x2f,0xfe,0x00,0x00,0x3f,0xfe,
	0x00,0x00,0x1f,0xfe,0x00,0x00,0x1f,0xfc,0x00,0x00,0x0f,0xfc,0x00,0x00,0x0f,
	0xfc,0x00,0x00,0x07,0xf8,0x00,0x00,0x07,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const unsigned char gHandCursorMaskData[] = 
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xf3,0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xe1,
	0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xe0,0x7f,0xff,0xff,0xe0,0x0f,0xff,0xff,
	0xe0,0x03,0xff,0xff,0xe0,0x01,0xff,0xfe,0x20,0x00,0xff,0xfe,0x00,0x00,0xff,
	0xfe,0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff,0x80,0x00,0xff,0xff,0x80,0x00,
	0xff,0xff,0xc0,0x00,0xff,0xff,0xc0,0x01,0xff,0xff,0xe0,0x01,0xff,0xff,0xe0,
	0x01,0xff,0xff,0xf0,0x03,0xff,0xff,0xf0,0x03,0xff,0xff,0xf0,0x03,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
/*{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DWORD MyGetSysColor(int nIndex)
{
	DWORD aResult = GetSysColor(nIndex);
	return ((aResult&0xff0000)>>16) | (aResult&0xff00)| ((aResult&0xff)<<16);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSWindowManager::MSWindowManager(HINSTANCE theHinstance)
{
	if(theHinstance==NULL)
		theHinstance = (HINSTANCE)GetModuleHandle(NULL);

	mHinstance = theHinstance;

	Cursor::SetStandardCursor(StandardCursor_Hand, new MSCursor(CreateCursor(mHinstance,12,4,32,32,gHandCursorMaskData,gHandCursorData),true));
	Cursor::SetStandardCursor(StandardCursor_Arrow, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW))));
	Cursor::SetStandardCursor(StandardCursor_IBeam, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_IBEAM))));
	Cursor::SetStandardCursor(StandardCursor_EastWest, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE))));
	Cursor::SetStandardCursor(StandardCursor_NorthSouth, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS))));
	Cursor::SetStandardCursor(StandardCursor_NorthSouthEastWest, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEALL))));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindowManager::AddImageDecoders()
{
	mImageDecoder.AddDecoder(new MSBMPDecoder,"bmp");
	WindowManager::AddImageDecoders();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindowManager::GetSystemColorScheme(ColorScheme *copyTo)
{
	copyTo->SetStandardColor(StandardColor_3DDarkShadow,MyGetSysColor(COLOR_3DDKSHADOW));
	copyTo->SetStandardColor(StandardColor_3DFace,MyGetSysColor(COLOR_3DFACE));
	copyTo->SetStandardColor(StandardColor_3DHilight,MyGetSysColor(COLOR_3DHILIGHT));
//	copyTo->SetStandardColor(StandardColor_3DHilight,MyGetSysColor(COLOR_3DLIGHT));
	copyTo->SetStandardColor(StandardColor_3DShadow,MyGetSysColor(COLOR_3DSHADOW));
	copyTo->SetStandardColor(StandardColor_Scrollbar,MyGetSysColor(COLOR_SCROLLBAR));

	copyTo->SetStandardColor(StandardColor_ButtonText,MyGetSysColor(COLOR_BTNTEXT));
	copyTo->SetStandardColor(StandardColor_GrayText,MyGetSysColor(COLOR_GRAYTEXT));
	copyTo->SetStandardColor(StandardColor_Hilight,MyGetSysColor(COLOR_HIGHLIGHT));
	copyTo->SetStandardColor(StandardColor_HilightText,MyGetSysColor(COLOR_HIGHLIGHTTEXT));

	copyTo->SetStandardColor(StandardColor_ToolTipBack,MyGetSysColor(COLOR_INFOBK));
	copyTo->SetStandardColor(StandardColor_ToolTipText,MyGetSysColor(COLOR_INFOTEXT));

	copyTo->SetStandardColor(StandardColor_MenuBack,MyGetSysColor(COLOR_MENU));
	copyTo->SetStandardColor(StandardColor_MenuText,MyGetSysColor(COLOR_MENUTEXT));

	copyTo->SetStandardColor(StandardColor_Back,MyGetSysColor(COLOR_WINDOW));
	copyTo->SetStandardColor(StandardColor_Text,MyGetSysColor(COLOR_WINDOWFRAME));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindowManager::GetNextEvent()
{
	MSG msg;
	if(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	else
		mTargetMsgLoopDepth = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindowManager::GetScreenRect(WONRectangle &theRect) 
{ 
	theRect.x = theRect.y = 0;
	theRect.width = GetSystemMetrics(SM_CXSCREEN);
	theRect.height = GetSystemMetrics(SM_CYSCREEN);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindowManager::Signal()
{
	MSWindow *aWindow = (MSWindow*)GetDefaultWindow();
	if(aWindow==NULL)
		return;

	if(aWindow->GetHWND()==NULL)
		return;

	::PostMessage(aWindow->GetHWND(),WM_USER,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSWindowManager::SetScreenResolutionHook(int theWidth, int theHeight)
{
	DEVMODE dm;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm );

	// Switch resolutions
	if (dm.dmPelsWidth!=theWidth || dm.dmPelsHeight!=theHeight)
	{
		dm.dmPelsWidth = theWidth;
		dm.dmPelsHeight = theHeight;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		ChangeDisplaySettings(&dm,CDS_FULLSCREEN);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindowManager::GetScreenResolutionHook(int &theWidth, int &theHeight)
{
	DEVMODE dm;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm );
	theWidth = dm.dmPelsWidth;
	theHeight = dm.dmPelsHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSWindow::MSWindow()
{
	mHwnd = NULL;
	mCurKeyMod = 0;
	mMinimizeOnAltTab = false;
	mForwardKeyMsgToParent = false;
	mLastNonFullPaintTick = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSWindow::~MSWindow()
{
	if(mHwnd!=NULL)
	{
		SetWindowLong(mHwnd, GWL_USERDATA, 0);
		DestroyWindow(mHwnd);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DisplayContext* MSWindow::DefDisplayContextAllocator(MSWindow *theWindow)
{
	return new MSDisplayContext(::GetDC(theWindow->GetHWND()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* MSWindow::DefFontAllocator(MSWindow *theWindow, const FontDescriptor &theDescriptor)
{
	return new MSFont(theWindow->GetDisplayContext(), theDescriptor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::CalcCodePage()
{
	char aBuf[32];
	unsigned short aLangId = ((WORD)GetKeyboardLayout(0))&0xffff;
	GetLocaleInfo(MAKELCID(aLangId,SORT_DEFAULT),LOCALE_IDEFAULTANSICODEPAGE,aBuf,32);
	mCurCodePage = atoi(aBuf);
	if(mCurCodePage==0)
		mCurCodePage = CP_ACP;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontPtr MSWindow::GetNativeFont(const FontDescriptor &theDescriptor)
{
	return mFontAllocator(this, theDescriptor); //new MSFont(mDisplayContext, theDescriptor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::Create(const WONRectangle& theWindowRectR)
{
	Window::Create(theWindowRectR);
	CalcCodePage();

	static int aCreateCount = 0;
	char aBuf[50];
	sprintf(aBuf,"WndClass%d",aCreateCount++);

	WNDCLASS wc;
	wc.lpszClassName = aBuf;
	wc.lpfnWndProc = StaticWindowProc;
	wc.style = CS_OWNDC | CS_DBLCLKS; /*| CS_VREDRAW | CS_HREDRAW;*/
	wc.hInstance = ((MSWindowManager*)mWindowManager)->GetHinstance();
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = NULL;//LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = NULL;//(HBRUSH)( COLOR_WINDOW+1 );
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	RegisterClass( &wc );

	HWND aParent = NULL;
	int aStyle = WS_OVERLAPPEDWINDOW;
	int aStyleEx = 0;

	if(mParent!=NULL)
	{
		aParent = ((MSWindow*)mParent)->mHwnd;
		aStyle |= WS_POPUPWINDOW;
//		aStyle |= WS_CHILD;
	}
	else
		aParent = gMSWindowDefaultOwnerHWND;

	if(mCreateFlags & CreateWindow_Popup)
	{
		aStyle = WS_POPUP;
//		aStyleEx = WS_EX_TOPMOST;
	}

	if(mCreateFlags & CreateWindow_NoSystemMenu) // Turns off the upper left icon and the upper right close button.
		aStyle &= ~(WS_SYSMENU);

	if(mCreateFlags & CreateWindow_NotMinimizable)
		aStyle &= ~(WS_MINIMIZEBOX);

	if(mCreateFlags & CreateWindow_NotSizeable)
		aStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

	int x = theWindowRectR.Left(), y = theWindowRectR.Top();
	if(mCreateFlags&CreateWindow_UseDefaultLocation)
		x = y = CW_USEDEFAULT;


	std::string aTitle = mTitle;
	HWND aWnd = CreateWindowEx(
		aStyleEx,
		aBuf,
		aTitle.c_str(),
		aStyle,
		x,
		y,
		theWindowRectR.Width(),
		theWindowRectR.Height(),
		aParent,
		NULL,
		((MSWindowManager*)mWindowManager)->GetHinstance(),
		this
   );
//	if(aParent!=NULL)
//	::SetParent(aWnd,aParent);

	if(aWnd && (mCreateFlags & CreateWindow_SizeSpecClientArea) && !(mCreateFlags & CreateWindow_Popup))
	{
		// Resize window so that client area is equal to the size specified
		
		WONRectangle aClientRect;
		WONRectangle aScreenRect;
		GetScreenPos(aScreenRect,false);
		GetScreenPos(aClientRect,true);

		int aWidthDelta = theWindowRectR.Width() - aClientRect.Width();
		int aHeightDelta = theWindowRectR.Height() - aClientRect.Height();
//		if(aWidthDelta<0)
//			aWidthDelta = aClientRect.Width() - theWindowRectR.Width();
//		if(aHeightDelta<0)
//			aHeightDelta = aClientRect.Height() - theWindowRectR.Height();

		int aTargetWidth = aScreenRect.Width() + aWidthDelta;
		int aTargetHeight = aScreenRect.Height() + aHeightDelta;

		Move(aScreenRect.Left(), aScreenRect.Top(), aTargetWidth, aTargetHeight);
	}

	if(mCreateFlags&CreateWindow_Centered)
		CenterWindow();

//	SetWindowLong(mHwnd, GWL_USERDATA, (LONG)this);

	if(!(mCreateFlags&CreateWindow_HideAtFirst))
	{
		mIsShowing = true;
		ShowWindowHook(true);
	}
	else
		mIsShowing = false;

	if(mIcon.get()!=NULL)
		SetIconHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::ClosePrv()
{
	Window::ClosePrv();

	if(mHwnd!=NULL)
	{
		DestroyWindow(mHwnd);
		mHwnd = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::SetTitleHook()
{
	std::string aTitle = mTitle;
	SetWindowText(mHwnd,aTitle.c_str());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::SetIconHook()
{
	HICON anIcon = NULL;
	if(mIcon.get()!=NULL)
		anIcon = ((MSNativeIcon*)mIcon.get())->GetIcon();

	SendMessage(mHwnd,WM_SETICON,(WPARAM)ICON_SMALL,(LPARAM)anIcon);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::Move(int x, int y, int width, int height)
{
	if(!IsOpen())
		return;

	if(width==-1 || height==-1)
	{
		WONRectangle aRect;
		GetScreenPos(aRect);
		if(width==-1)
			width = aRect.Width();

		if(height==-1)
			height = aRect.Height();
	}

	MoveWindow(mHwnd, x, y, width, height, TRUE);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::GetScreenPos(WONRectangle &theRectR, bool clientArea)
{
	RECT aRect;
	if(clientArea)
	{
		GetClientRect(mHwnd, &aRect);
		POINT aPoint = { aRect.left, aRect.top };
		ClientToScreen(mHwnd,&aPoint);
		theRectR.x = aPoint.x;
		theRectR.y = aPoint.y;
	}
	else
	{
		GetWindowRect(mHwnd,&aRect);
		theRectR.x = aRect.left;
		theRectR.y = aRect.top;
	}

	theRectR.width = aRect.right - aRect.left;
	theRectR.height = aRect.bottom - aRect.top;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::SetFocus()
{
	::SetFocus(mHwnd);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::ShowWindowHook(bool show)
{
	int aCmd = SW_SHOW;
	if(mIsPopup)
		aCmd = SW_SHOWNA;

	if(!show)
		aCmd = SW_HIDE;


	::ShowWindow(mHwnd, aCmd);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::MinimizeWindowHook(bool minimize)
{
	int aCmd = minimize?SW_MINIMIZE:SW_RESTORE;
	::ShowWindow(mHwnd, aCmd);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::SetTopMostHook(bool topMost)
{
	if (topMost)
		::SetWindowPos(mHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		::SetWindowPos(mHwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::SetNativeCursor(Cursor *theCursor)
{
	SetCursor(((MSCursor*)theCursor)->mNativeCursor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::ShowNativeCursorHook(bool show)
{
	if(!show)
		SetCursor(NULL);
//	int aResult = ShowCursor(show);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSWindow::IsShowing()
{
	return ::IsWindowVisible(mHwnd)?true:false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MSWindow::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MSWindow *aWindow = (MSWindow*)GetWindowLong(hwnd, GWL_USERDATA);
	if(aWindow==NULL)
	{
		if(msg==WM_CREATE || msg==WM_NCCREATE)
		{
			CREATESTRUCT *aStruct = (CREATESTRUCT*)lParam;
			aWindow = (MSWindow*)aStruct->lpCreateParams;
			aWindow->mHwnd = hwnd;
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)aWindow);
		}
		else
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
		
	return aWindow->WindowProc(hwnd, msg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::MyGetKeyState()
{
	int aState;
	 
	aState = (GetKeyState(KEYCODE_SHIFT)>>15)&1;
	mCurKeyMod = aState;

	aState = (GetKeyState(KEYCODE_ALT)>>15)&1;
	mCurKeyMod |= (aState<<1);

	aState = (GetKeyState(KEYCODE_CONTROL)>>15)&1;
	mCurKeyMod |= (aState<<2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::PostMouseEvent(WindowEventType theType, int theButtonOrWheelAmount, LPARAM lParam)
{
	PostEvent(new MouseEvent(theType, (short)LOWORD(lParam), (short)HIWORD(lParam), mCurKeyMod, theButtonOrWheelAmount));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::SetMouseCapture(bool on)
{
	if(on)
		SetCapture(mHwnd);
	else
		ReleaseCapture();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::DoMinimizeOnAltTab()
{
	HWND aWnd = mHwnd;
	HWND aParent = ::GetParent(mHwnd);
	while(aParent!=NULL)
	{
		aWnd = aParent;
		aParent = ::GetParent(mHwnd);
	}
	
	ShowWindow(aWnd, SW_MINIMIZE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SendMsgToParent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND aParent = ::GetParent(hwnd);
	if(aParent!=NULL)
		PostMessage(aParent,msg,wParam,lParam);
}

#define ForwardKeyMsg if(mForwardKeyMsgToParent) SendMsgToParent(hwnd,msg,wParam,lParam)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef HIMC (WINAPI *ImmGetContextFunc)(HWND);
typedef BOOL (WINAPI *ImmReleaseContextFunc)(HWND, HIMC);
typedef LONG (WINAPI *ImmGetCompositionStringWFunc)(HIMC, DWORD, LPVOID, DWORD);

static ImmGetContextFunc gImmGetContextFunc = NULL;
static ImmReleaseContextFunc gImmReleaseContextFunc = NULL;
static ImmGetCompositionStringWFunc gImmGetCompositionStringWFunc = NULL;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class T>
static bool GetWinIMEFunc(HMODULE theModule, const char *theName, T &theFunc)
{
	theFunc = (T)GetProcAddress(theModule,theName);
	return theFunc!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TryLoadWinIMEFunctions()
{
	struct AutoUnloadLibrary
	{
		HMODULE mModule;

		AutoUnloadLibrary() { mModule = NULL; }
		~AutoUnloadLibrary() { if(mModule!=NULL) FreeLibrary(mModule); }

	};
	static AutoUnloadLibrary gIMEDLL;
	HMODULE aModule = LoadLibrary("Imm32.dll");
	gIMEDLL.mModule = aModule;
	if(aModule==NULL)
		return false;

	if(!GetWinIMEFunc(aModule,"ImmGetContext",gImmGetContextFunc)) return false;
	if(!GetWinIMEFunc(aModule,"ImmReleaseContext",gImmReleaseContextFunc)) return false;
	if(!GetWinIMEFunc(aModule,"ImmGetCompositionStringW",gImmGetCompositionStringWFunc)) return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool LoadWinIMEFunctions()
{
	static bool gTriedLoadFunctions = false;
	static bool gLoadFunctionsResult = false;

	if(!gTriedLoadFunctions)
		gLoadFunctionsResult = TryLoadWinIMEFunctions();

	return gLoadFunctionsResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LRESULT MSWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	bool forceProcessEvents = true;
	switch(msg)
	{
		case WM_ACTIVATE:
			if(LOWORD(wParam)!=WA_INACTIVE && mWindowManager->NotModalWindow(this))
			{
				MSWindow *aModalWindow = (MSWindow*)mWindowManager->GetModalWindow();
//				FlashWindow(aModalWindow->mHwnd,TRUE);
				ShowWindow(aModalWindow->mHwnd,SW_SHOW);
				return 0;
			}
			else
				return DefWindowProc(hwnd,msg,wParam,lParam);
			break;

		case WM_CLOSE:
			Close();
			return 1;

		case WM_MOUSEACTIVATE:
			if(mIsPopup)
				return MA_NOACTIVATE;
			else
				return DefWindowProc(hwnd,msg,wParam,lParam);
			break;

		case WM_NCLBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCRBUTTONDBLCLK:
			EndPopup();
			if(mWindowManager->NotModalWindow(this))
				return 0;
			else
				return DefWindowProc(hwnd,msg,wParam,lParam);
			break;

		case WM_CREATE:
			OnCreate();
		break;
		case WM_SYSCOLORCHANGE:
//			((MSWindowManager*)mWindowManager)->SetSystemColors();
//			mRootContainer->Invalidate();
			break;

		case WM_SIZE:
			if(wParam==SIZE_RESTORED || wParam==SIZE_MAXIMIZED)
				PostEvent(new SizeEvent(-1,-1,(short)LOWORD(lParam), (short)HIWORD(lParam)));
			break;

		case WM_MOVE:
			PostEvent(new SizeEvent((short)LOWORD(lParam), (short)HIWORD(lParam),-1,-1));
			break;

		case WM_MOUSEMOVE:		PostMouseEvent(InputEvent_MouseMove, 0, lParam);break;

		case WM_LBUTTONDOWN:	PostMouseEvent(InputEvent_MouseDown, MouseButton_Left, lParam); break; //SetCapture(hwnd); break; 
		case WM_LBUTTONDBLCLK:	PostMouseEvent(InputEvent_MouseDoubleClick,	MouseButton_Left, lParam); break;
		case WM_LBUTTONUP:		PostMouseEvent(InputEvent_MouseUp, MouseButton_Left, lParam); break; //ReleaseCapture(); break;

		case WM_MBUTTONDOWN:	PostMouseEvent(InputEvent_MouseDown, MouseButton_Middle, lParam); break; 
		case WM_MBUTTONDBLCLK:	PostMouseEvent(InputEvent_MouseDoubleClick,	MouseButton_Middle, lParam); break;
		case WM_MBUTTONUP:		PostMouseEvent(InputEvent_MouseUp, MouseButton_Middle, lParam); break;

		case WM_RBUTTONDOWN:	PostMouseEvent(InputEvent_MouseDown, MouseButton_Right, lParam); break; 
		case WM_RBUTTONDBLCLK:	PostMouseEvent(InputEvent_MouseDoubleClick,	MouseButton_Right, lParam); break;
		case WM_RBUTTONUP:		PostMouseEvent(InputEvent_MouseUp,		MouseButton_Right, lParam); break;

		case WM_MOUSEWHEEL:	
		{
			// Mouse Wheel is in screen coordinates.
			WONRectangle aRect;
			GetScreenPos(aRect,true); 
			short mx = LOWORD(lParam); 
			short my = HIWORD(lParam);
			lParam = MAKELPARAM(mx - aRect.Left(), my - aRect.Top());
			PostMouseEvent(InputEvent_MouseWheel,	(short)HIWORD(wParam), lParam); 
			break;
		}

		case WM_INPUTLANGCHANGE: 
		{
			CalcCodePage();
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

		case WM_IME_COMPOSITION:
			if(!LoadWinIMEFunctions())
				return DefWindowProc(hwnd,msg,wParam,lParam); 

			if (lParam & GCS_RESULTSTR) 
			{
				// Note: would work for NT too
				HIMC hIMC = gImmGetContextFunc(hwnd);
				if (hIMC) 
				{
					DWORD dwSize = gImmGetCompositionStringWFunc(hIMC, GCS_RESULTSTR, NULL, 0);
					std::wstring aStr;
					aStr.resize(dwSize/2);
					gImmGetCompositionStringWFunc(hIMC, GCS_RESULTSTR, (char*)aStr.data(),dwSize); 
					for(int i=0; i<aStr.length(); i++)
						PostEvent(new KeyEvent(InputEvent_KeyChar, aStr.at(i), mCurKeyMod)); 
				}
				gImmReleaseContextFunc(hwnd, hIMC);

			}
			else 
				return DefWindowProc(hwnd,msg,wParam,lParam); 
	
			break;

		case WM_IME_CHAR:			
		{
			wchar_t aChar = wParam;
			MultiByteToWideChar(mCurCodePage,0,(const char*)&wParam,2,&aChar,1);
			PostEvent(new KeyEvent(InputEvent_KeyChar, aChar, mCurKeyMod)); 
			break;
		}

		case WM_CHAR:			
		{
			wchar_t aChar = wParam;
			MultiByteToWideChar(mCurCodePage,0,(const char*)&wParam,2,&aChar,1);
			PostEvent(new KeyEvent(InputEvent_KeyChar, aChar, mCurKeyMod)); 
			break;
		}

		case WM_KEYDOWN:		MyGetKeyState(); PostEvent(new KeyEvent(InputEvent_KeyDown,	wParam, mCurKeyMod)); ForwardKeyMsg; break;
		case WM_KEYUP:			MyGetKeyState(); PostEvent(new KeyEvent(InputEvent_KeyUp, wParam, mCurKeyMod)); ForwardKeyMsg; break;
		
		// need WM_SYSKEY messages to detect the alt key being pressed
		case WM_SYSKEYDOWN:	
		case WM_SYSKEYUP:		
			if(wParam==KEYCODE_TAB && mMinimizeOnAltTab)
			{
				DoMinimizeOnAltTab();
				return 1;
			}

			MyGetKeyState(); 
			ForwardKeyMsg;
			return DefWindowProc(hwnd,msg,wParam,lParam); 


		case WM_KILLFOCUS:		PostEvent(new InputEvent(InputEvent_LostFocus)); break;
		case WM_SETFOCUS:		MyGetKeyState(); PostEvent(new InputEvent(InputEvent_GotFocus)); break;

		case WM_DESTROY:
			KillTimer(mHwnd,0);
			SetWindowLong(hwnd, GWL_USERDATA, 0);
			mHwnd = NULL;
			Close();
			forceProcessEvents = true;
			break;

		case WM_TIMER:
			forceProcessEvents = true;
			break;

		case WM_SIZING:
		{
			LPRECT aRect = (LPRECT) lParam;

			// Min
			// Width
			if (aRect->right - aRect->left < mMinWidth)
			{
				aRect->right = aRect->left + mMinWidth; 
			}
			// Height
			if ( aRect->bottom - aRect->top < mMinHeight)
			{
				aRect->bottom = aRect->top + mMinHeight;
			}

			// Max
			// Width
			if (aRect->right - aRect->left > mMaxWidth)
			{
				aRect->right = aRect->left + mMaxWidth;
			}
			// Height
			if ( aRect->bottom - aRect->top > mMaxHeight)
			{
				aRect->bottom = aRect->top + mMaxHeight;
			}
		}
		break;

		case WM_SYSCOMMAND:
		{
			if (wParam == SC_MAXIMIZE) // Make sure the window is never larger than max size
			{
				int aMaximizedWidth = GetSystemMetrics(SM_CXMAXIMIZED);
				int aMaximizedHeight = GetSystemMetrics(SM_CYMAXIMIZED);

				// Handle the case where the max size is smaller than the screen size 
				if (mMaxWidth < aMaximizedWidth || mMaxHeight < aMaximizedHeight)
				{
					DefWindowProc(hwnd,msg,wParam,lParam); // This is done so the icon will change
					int x, y, cx, cy;
					// Width
					if (mMaxWidth < aMaximizedWidth)
					{
						x =  (aMaximizedWidth - mMaxWidth) / 2;
						cx = mMaxWidth;
					}
					else
					{
						x = 0;
						cx = aMaximizedWidth;
					}
					// Height
					if (mMaxHeight < aMaximizedHeight)
					{
						y =  (aMaximizedHeight - mMaxHeight) / 2;
						cy = mMaxHeight;
					}
					else
					{
						y = 0;
						cy = aMaximizedHeight;
					}
					
					SetWindowPos(hwnd,NULL,x,y,cx,cy,SWP_NOZORDER);
				}
				else // Maximize as normal
				{
					return DefWindowProc(hwnd,msg,wParam,lParam);
				}
			}
	/*		else if(wParam == SC_MINIMIZE && GetParent()!=NULL)
			{
				HWND aParent = ((MSWindow*)GetParent())->GetHWND();
				PostMessage(aParent,msg,wParam,0);
				Show(false);
				return 0;
			}*/
			else
			{
				return DefWindowProc(hwnd,msg,wParam,lParam);
			}
		}
		break;

		case WM_PAINT:
			OnPaint();
		break;

		case WM_USER: // used to wake the window up
			forceProcessEvents = true;
			break;

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	if(forceProcessEvents || GetQueueStatus(QS_ALLINPUT)==0)
		mWindowManager->ProcessEvents();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSWindow::MessageLoopHook()
{
	if(!Window::MessageLoopHook())
		return false;

	if(mLastNonFullPaintTick!=0 && GetTickCount()-mLastNonFullPaintTick>=1000)
	{
		mLastNonFullPaintTick = 0;
		InvalidateRect(mHwnd,NULL,FALSE); // reblit whole window every once and a while because windows sometimes loses track of which parts of the window are actually invalid (usually when other apps are being dragger over the window... I wrote a simple test app to confirm this and also verified it on Notepad's menu bar.)
	}
	else if(mRootContainer->IsInvalid())
	{
		WONRectangle &r = mRootContainer->GetInvalidRect();
		RECT aRect = { r.Left(), r.Top(), r.Right(), r.Bottom() };
		InvalidateRect(mHwnd,&aRect,FALSE);
//		InvalidateRect(hwnd,NULL,FALSE);
	}

	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::TimerHook(DWORD theElapsed)
{
	Window::TimerHook(theElapsed);

	if(mMouseIsInClientArea /*&& !mMouseIsDown*/)
	{
		POINT aPoint;
		GetCursorPos(&aPoint);
		HWND hwnd = WindowFromPoint(aPoint);
		ScreenToClient(mHwnd, &aPoint);
		if(hwnd!=mHwnd || !mRootContainer->Contains(aPoint.x,aPoint.y))
		{
//			mMouseX = aPoint.x; // I do this in case anybody need to check the mouse coords on MouseExit
//			mMouseY = aPoint.y; 
			if(hwnd!=mHwnd)
			{
				WindowManager *aManager = GetWindowManager(); 
				MSWindow *aWindow = (MSWindow*)aManager->GetPopupWindow();
				if(aWindow!=NULL && aManager->PopupFlagSet(PopupFlag_DontWantMouse) && aWindow->mHwnd==hwnd)
					return;
			}
			
			PostEvent(new InputEvent(InputEvent_MouseExit));
/*
			if(!mNativeCursorShowing)
				ShowCursor(true); // show cursor when it leaves the window

			mMouseIsInClientArea = false;
			mRootContainer->MouseExit();
			mCurrentCursor = NULL;*/
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::OnCreate()
{
	mDisplayContext = mDisplayContextAllocator(this); //new MSDisplayContext(::GetDC(mHwnd));
	mOffscreenImage = CreateImage(mRootContainer->Width(), mRootContainer->Height());
//	mRootContainer->SetPosSize(0,0,640,480);
	SetTimer(mHwnd,0,50,NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSWindow::OnPaint()
{
	PAINTSTRUCT ps;
	HDC aDC = BeginPaint( mHwnd, &ps );
	RECT &aRect1 = ps.rcPaint;

	if(mRootContainer->IsInvalid())
	{
		// We need to check if more of the root is invalid than the window.  If this is true
		// then we need to invalidate the extra space.  This can happen when we recursively enter
		// the message loop and immediately get a paint call since the window gets invalidated at 
		// the bottom of the message loop.
		WONRectangle &aWONRect = mRootContainer->GetInvalidRect();
		RECT aRect2 = { aWONRect.Left(), aWONRect.Top(), aWONRect.Right(), aWONRect.Bottom() };
		if(aRect2.left<aRect1.left || aRect2.top<aRect1.top || aRect2.right>aRect1.right || aRect2.bottom>aRect1.bottom)
			InvalidateRect(mHwnd, &aRect2, FALSE);

		mRootContainer->PrePaint(mOffscreenImage->GetGraphics());
		mRootContainer->Paint(mOffscreenImage->GetGraphics());
	}

//	HDC anOldDC = mDisplayContext->mDC;
//	mDisplayContext->Attach(aDC);	// attach the paint DC for proper clipping
	if(mLastNonFullPaintTick==0 && !(aRect1.left<=0 && aRect1.top<=0 && aRect1.right>=mRootContainer->Right() && aRect1.bottom>=mRootContainer->Bottom()))
		mLastNonFullPaintTick = GetTickCount();

	mOffscreenImage->DrawEntire(mDisplayContext,0,0);
//	mDisplayContext->Attach(anOldDC);

	EndPaint( mHwnd, &ps );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
