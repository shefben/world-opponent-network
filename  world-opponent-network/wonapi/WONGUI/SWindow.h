#ifndef __WON_SWINDOW_H__
#define __WON_SWINDOW_H__
#include "Window.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SWindow : public Window
{
protected:
	virtual ~SWindow();

	virtual FontPtr GetNativeFont(const FontDescriptor &theDescriptor);
	virtual void SetNativeCursor(Cursor *theCursor);
	virtual void ShowNativeCursorHook(bool show);
	virtual void SetMouseCapture(bool on);

	int mCurKeyMod;

public:
	typedef DisplayContext*(*DisplayContextAllocator)();
	typedef Font*(*FontAllocator)(const FontDescriptor &theDescriptor);
	typedef void(*NativeCursorFunc)(Cursor *theCursor, bool show);
	typedef void(*MouseCaptureFunc)(bool capture);

	static void SetDisplayContextAllocator(DisplayContextAllocator theAllocator);
	static void SetFontAllocator(FontAllocator theAllocator);
	static void SetNativeCursorFunc(NativeCursorFunc theFunc);
	static void SetMouseCaptureFunc(MouseCaptureFunc theFunc);

	void NotifyFocus(bool gotFocus);
	void NotifyKeyMod(int theKeyMod);
	void NotifyKeyUp(int theKey);
	void NotifyKeyDown(int theKey);
	void NotifyKeyChar(int theKey);
	void NotifyMouseMove(int x, int y);
	void NotifyMouseDown(MouseButton theButton);
	void NotifyMouseDoubleClick(MouseButton theButton);
	void NotifyMouseUp(MouseButton theButton);
	void NotifyMouseWheel(int theAmount);
	void NotifySize(int theWidth, int theHeight);



public:
	SWindow();

	virtual void Create(const WONRectangle& theWindowRectR);
	virtual void SetFocus();
};
typedef SmartPtr<SWindow> SWindowPtr;

} // namespace WONAPI

#endif