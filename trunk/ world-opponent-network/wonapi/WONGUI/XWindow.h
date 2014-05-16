#ifndef __WON_XWINDOW_H__
#define __WON_XWINDOW_H__

#include "Window.h"
#include <map>
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{
class XWindow;
typedef SmartPtr<XWindow> XWindowPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XWindowManager : public WindowManager
{
protected:
	Display *mDisplay;
	typedef std::map<int,XWindow*> WindowMap;
	WindowMap mWindowMap;
	bool mProcessingEvent;

	virtual ~XWindowManager()  { } 

	static void sig_alarm_handler(int);

	virtual void GetNextEvent();
	virtual bool AddWindow(Window *theWindow);
	virtual bool RemoveWindow(Window *theWindow);

public:
	XWindowManager(Display *theDisplay = NULL);

	Display* GetDisplay() { return mDisplay; }

	virtual void GetScreenRect(WONRectangle &theRect);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XWindow : public Window
{
protected:
	friend class XWindowManager;
	KeySym *mKeyMap;
	Display *mDisplay;
	::Window mWindow;
	bool mIsMapped;
	int mOldX, mOldY, mOldWidth, mOldHeight;

	virtual FontPtr GetNativeFont(const FontDescriptor &theDescriptor);
	virtual void SetNativeCursor(Cursor *theCursor);
	virtual void ShowNativeCursorHook(bool show);
	virtual void SetTitleHook();

protected:
	int WindowProc(XEvent &theEvent);
	virtual bool MessageLoopHook();

	virtual ~XWindow();
	virtual void ClosePrv();

public:
	XWindow();

	virtual void Create(const WONRectangle& theWindowRectR);
	virtual void Move(int x, int y, int width = -1, int height = -1);
	virtual void GetScreenPos(WONRectangle &theRectR, bool clientArea = false);
	virtual void SetFocus();
	virtual void ShowWindowHook(bool show);
	virtual void SetMouseCapture(bool on);

};

};
#endif
