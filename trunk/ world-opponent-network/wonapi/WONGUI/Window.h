#ifndef __WON_WINDOW_H__
#define __WON_WINDOW_H__

#include "RootContainer.h"
#include "Keyboard.h"
#include "NativeImage.h"
#include "Animation.h"
#include "GUISystem.h"
#include "WindowEvent.h"
#include "WindowManager.h"
#include "WONCommon/CriticalSection.h"

#include <list>
#include <set>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum CreateWindowFlags
{
	CreateWindow_Centered			= 0x0001,
	CreateWindow_HideAtFirst		= 0x0002,
	CreateWindow_NoSystemMenu		= 0x0004,
	CreateWindow_NotMinimizable		= 0x0008,
	CreateWindow_NotSizeable		= 0x0010,
	CreateWindow_Popup				= 0x0020,
	CreateWindow_SizeSpecClientArea = 0x0040, // Only implemented in MSWindow at present
	CreateWindow_UseDefaultLocation = 0x0080
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Window;
typedef SmartPtr<Window> WindowPtr;

class Window : public RefCount
{
protected:
	enum WindowState 
	{
		WindowState_Open,
		WindowState_Closing,
		WindowState_Closed
	};
	WindowState mWindowState;
		
	NativeImagePtr mOffscreenImage;
	int mMouseX, mMouseY;
	int mKeyMod;

	bool mMouseIsDown[3];
	bool mMouseIsInClientArea;
	Cursor* mCurrentCursor;
	bool mNativeCursorShowing;
	DisplayContextPtr mDisplayContext;

	// Double-Click detection
	DWORD mButtonDownTick[3];
	bool mButtonDoubleClick[3];
	bool mDetectDoubleClick;
	int mDoubleClickX, mDoubleClickY;

	// Mouse-Hover detection
	DWORD mLastMoveTick;

	// Min window size
	int mMinWidth;
	int mMinHeight;

	// Max window size
	int mMaxWidth;
	int mMaxHeight;

	// Resizing
	bool mNeedResize;
	int mNewWidth;
	int mNewHeight;
	DWORD mResizeDelay;
	DWORD mLastResizeTick;
	DWORD mLastCheckCursorTick;
	int mSuspendRootResize;

	// Create Flags
	int mCreateFlags;

	bool mIsPopup;
	bool mIsOneComponentWindow;
	bool mMinimizeOnResolutionRestore;

	friend class WindowManager;
	WindowManager *mWindowManager;
	Window* mParent;
	typedef std::set<Window*> ChildWindowSet;
	ChildWindowSet mChildWindowSet;

	WindowEventBasePtr mCloseEvent;
	typedef bool(*AllowCloseFunc)(Window *theWindow);
	AllowCloseFunc mAllowCloseFunc;

	NativeIconPtr mIcon;

	GUIString mTitle;
	bool mIsShowing;

protected:
	virtual ~Window();

public:
	void HandleMouseEvent(MouseEvent *theEvent);
	void HandleKeyEvent(KeyEvent *theEvent);
	void HandleInputEvent(InputEvent *theEvent);
	void HandleMouseExit();
	void HandleMouseDown(MouseEvent *theEvent);

protected:
	RootContainerPtr mRootContainer;
	FontMap mFontMap;
	FontPtr mDefaultFont;

	virtual FontPtr GetNativeFont(const FontDescriptor &/*theDescriptor*/) { return NULL; }
	virtual bool MessageLoopHook();
	virtual void TimerHook(DWORD theElapsed);

	void CheckSetCursor(bool force = false);
	virtual void SetTitleHook();
	virtual void SetIconHook();
	virtual void SetNativeCursor(Cursor *theCursor);
	virtual void ShowNativeCursorHook(bool show);
	virtual void ShowWindowHook(bool show);
	virtual void MinimizeWindowHook(bool minimize);
	virtual void SetTopMostHook(bool topMost);
	void ShowNativeCursor(bool show);
	void CheckResize(DWORD theTick);

	virtual void ClosePrv();

	void SetModal(bool isModal);

public:
	Window();

	void SetCreateFlags(int theFlags) { mCreateFlags = theFlags; }
	void ChangeCreateFlags(int theFlags, bool theAdd){mCreateFlags = theAdd?theFlags|mCreateFlags:!theFlags&mCreateFlags;}
	void SetParent(Window *theParent);
	Window* GetParent() { return mParent; }

	virtual void Create(const WONRectangle& theWindowRectR);
	bool CanClose();
	bool Close();
	void SetTitle(const GUIString &theTitle);
	void SetIcon(NativeIcon *theIcon);
	void Resize(int width, int height);
	virtual void Move(int x, int y, int width = -1, int height = -1);
	virtual void GetScreenPos(WONRectangle& theRectR, bool clientArea = false);
	virtual void SetFocus();
	virtual void Show(bool show);
	void Minimize(bool minimize);

	void SetTopMost(bool topMost);
	virtual void SetMouseCapture(bool on);
	void CenterWindow(bool inParent = true);
	
	void AddComponent(Component *theComponent);
	void RemoveComponent(Component *theComponent);

	NativeImage* GetOffscreenImage();
	NativeImagePtr CreateImage(int theWidth, int theHeight);

	Graphics& GetGraphics();
	FontPtr GetFont(const FontDescriptor &theDescriptor);

	int GetMouseX() { return mMouseX; }
	int GetMouseY() { return mMouseY; }
	int GetKeyMod() { return mKeyMod; }

	static Window* GetDefaultWindow();
	static RawImage32Ptr CreateRawImage32(int theWidth, int theHeight);
	static RawImage8Ptr CreateRawImage8(int theWidth, int theHeight, Palette *thePalette = NULL);
	static RawImage1Ptr CreateRawImage1(int theWidth, int theHeight);

	RootContainer* GetRoot() { return mRootContainer; }
	DisplayContext* GetDisplayContext() { return mDisplayContext; }
	WindowManager* GetWindowManager() { return mWindowManager; }  
	NativeImagePtr DecodeImage(const char *theFilePath);
	RawImagePtr DecodeImageRaw(const char *theFilePath);
	static void SetSkipDecodeImages(bool skip); // useful for debugging slow app loading times

	virtual void PostEvent(WindowEventBase *theEvent);

	void SetMinWindowSize(int theMinWidth, int theMinHeight) {mMinWidth = theMinWidth; mMinHeight = theMinHeight; }
	void SetMaxWindowSize(int theMaxWidth, int theMaxHeight) {mMaxWidth = theMaxWidth; mMaxHeight = theMaxHeight; }

	void SetIsOneComponentWindow(bool theVal) { mIsOneComponentWindow = theVal; }
	bool IsOneComponentWindow() { return mIsOneComponentWindow; }
	bool HasFocus() { return GetWindowManager()->GetFocusWindow()==this; } // mRootContainer->HasFocus(); } 
	bool MouseIsInClientArea() { return mMouseIsInClientArea; }
	bool CheckDoubleClick(MouseButton theButton); // true if button was double clicked on last mouse down
	bool IsMouseDown(MouseButton theButton);

	int DoDialog(Component *theComponent = NULL);
	void EndDialog(int theResult = -1, Component *theDialog = NULL);
	void DoPopup(const PopupParams &theParams);
	void EndPopup(Component *thePopup = NULL);
	bool CheckDialog(InputEvent *theEvent);
	void SuspendRootResize(bool suspend);

	bool IsOpen() { return mWindowState == WindowState_Open; }
	virtual bool IsShowing() { return mIsShowing; }
	const GUIString& GetTitle() { return mTitle; }

	void SetMinimizeOnResolutionRestore(bool minimize) { mMinimizeOnResolutionRestore = minimize; }
	void SetCloseEvent(WindowEventBase *theEvent) { mCloseEvent = theEvent; }
	void SetAllowCloseFunc(AllowCloseFunc theFunc) { mAllowCloseFunc = theFunc; }
	void ReMouseMove(); // simulates a mouse move to the current location
	void Invalidate();
};

}; // namespace WONAPI

#endif