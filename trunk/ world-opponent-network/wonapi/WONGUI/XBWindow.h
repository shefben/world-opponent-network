#ifndef __WON_XBWINDOW_H__
#define __WON_XBWINDOW_H__

#include "Window.h"
#include "XBInput.h"
#include "XBGraphics.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{

class XBWindow : public Window
{
protected:
	virtual FontPtr GetNativeFont(const FontDescriptor &theDescriptor);

protected:
	static IDirect3DDevice8*	mDevice;
	static XBInput				mInput;

	XBGraphics					mGraphics;

	static int StaticMessageLoop();
	int WindowProc();



public:
	XBWindow();
	virtual ~XBWindow();

	virtual void Create(const WONRectangle& theWindowRectR);
	virtual void Close();

	static void Init(IDirect3DDevice8 *theDevice);
};

};
#endif
