#ifndef __MSWINDOW_H__
#define __MSWINDOW_H__

#include <assert.h>
#include "Window.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSWindowManager : public WindowManager
{
protected:
	HINSTANCE mHinstance;

	virtual ~MSWindowManager() { }
	virtual void GetNextEvent();
	virtual void AddImageDecoders();

public:
	MSWindowManager(HINSTANCE theHinstance = NULL);
	HINSTANCE GetHinstance() { return mHinstance; }

	void SetSystemColors();
	void GetSystemColorScheme(ColorScheme *copyTo);
	virtual void GetScreenRect(WONRectangle &theRect);
	virtual void Signal();
	virtual bool SetScreenResolutionHook(int theWidth, int theHeight);
	virtual void GetScreenResolutionHook(int &theWidth, int &theHeight);

private:
	// Dummy Copy Constructor (removes warning and prevents nasty accidents).
	MSWindowManager(MSWindowManager&)
	{
		assert(false);
	}

	// Dummy Assignment operator (removes warning and prevents nasty accidents).
	MSWindowManager& operator= (MSWindowManager&)
	{
		assert(false);
		return *this;
	}
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSWindow : public Window
{
protected:
	HWND mHwnd;
	DWORD mLastNonFullPaintTick;
	int mCurKeyMod;
	int mCurCodePage;
	bool mMinimizeOnAltTab;
	bool mForwardKeyMsgToParent;

	typedef DisplayContext*(*DisplayContextAllocator)(MSWindow *theWindow);
	typedef Font*(*FontAllocator)(MSWindow *theWindow, const FontDescriptor &theDescriptor);

	static DisplayContextAllocator mDisplayContextAllocator;
	static FontAllocator mFontAllocator;
	static DisplayContext* DefDisplayContextAllocator(MSWindow *theWindow);
	static Font* DefFontAllocator(MSWindow *theWindow, const FontDescriptor &theDescriptor);

	virtual ~MSWindow();
	void CalcCodePage();

protected:
	void MyGetKeyState();
	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void OnPaint();
	virtual void OnCreate();

	virtual FontPtr GetNativeFont(const FontDescriptor &theDescriptor);
	virtual void SetNativeCursor(Cursor *theCursor);
	virtual void ShowNativeCursorHook(bool show);
	virtual void SetTitleHook();
	virtual void SetIconHook();

	virtual bool MessageLoopHook();
	virtual void TimerHook(DWORD theElapsed);


	void PostMouseEvent(WindowEventType theType, int theButtonOrWheelAmount, LPARAM lParam);
	virtual void ClosePrv();

public:
	MSWindow();

	virtual void Create(const WONRectangle& theWindowRectR);
	virtual void Move(int x, int y, int width = -1, int height = -1);
	virtual void GetScreenPos(WONRectangle &theRectR, bool clientArea = false);
	virtual void SetFocus();
	virtual void ShowWindowHook(bool show);
	virtual void MinimizeWindowHook(bool minimize);
	virtual void SetTopMostHook(bool topMost);
	virtual void SetMouseCapture(bool on);
	virtual bool IsShowing();

	void DoMinimizeOnAltTab();
	void SetMinimizeOnAltTab(bool minimize) { mMinimizeOnAltTab = minimize; }
	void SetForwardKeyMsgToParent(bool forward) { mForwardKeyMsgToParent = forward; } // Helps DirectX catch alt-tab switches

	HWND GetHWND() { return mHwnd; } // obviously non-portable
	static void SetDefaultOwnerHWND(HWND theHWND); // uses this HWND for parent in CreateWindow call if mParent is NULL
	static void SetDisplayContextAllocator(DisplayContextAllocator theAllocator) { mDisplayContextAllocator = theAllocator; }
	static void SetFontAllocator(FontAllocator theAllocator) { mFontAllocator = theAllocator; }
};
typedef SmartPtr<MSWindow> MSWindowPtr;

}; // namespace WONAPI


#endif