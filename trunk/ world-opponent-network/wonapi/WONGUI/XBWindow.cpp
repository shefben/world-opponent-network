#include "XBWindow.h"
#include "XBFont.h"

using namespace WONAPI;

IDirect3DDevice8* XBWindow::mDevice = NULL;
XBInput			  XBWindow::mInput;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBWindow::Init(IDirect3DDevice8 *theDevice)
{
	mDevice = theDevice;
	mMessageLoopFunc = StaticMessageLoop;

	// Only using max 2 controllers for now
	mInput.InitDevices(2);
	

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBWindow::XBWindow()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBWindow::~XBWindow()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int XBWindow::StaticMessageLoop()
{
	return ((XBWindow*)mDefaultWindow)->WindowProc();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontPtr XBWindow::GetNativeFont(const FontDescriptor &theDescriptor)
{
	return new XBFont(mDisplayContext, theDescriptor);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBWindow::Create(const WONRectangle& r)
{
	mDisplayContext = new DisplayContext;
	mDisplayContext->mDevice = mDevice;

	mGraphics.Attach(mDisplayContext);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBWindow::Close()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int XBWindow::WindowProc()
{
	while( TRUE )
    {
		// Take input
		mInput.GetInput();
		// if mInput.IsNewInput() do something
		if (mInput.mGamepad[0].wPressedButtons)
		{
			char buf[255];
			sprintf(buf, "Button(s) %d pressed\n", mInput.mGamepad[0].wPressedButtons);
			
			OutputDebugString(buf);
		}

		// Analog buttons
		for (int i = 0; i < 8; ++i)
		{
			if (mInput.mGamepad[0].bPressedAnalogButtons[i])
			{
				char buf[255];
				sprintf(buf, "Analog Button(s) %d pressed\n", i);
			
				OutputDebugString(buf);
			}
		}

		// Sticks
		{
			//#1
			//if (mInput.mGamepad[0].fX1 < 0.25 || mInput.mGamepad[0].fX2 > 0.25)

			//#2

		}

		// Handle timer events
		DWORD aTick = GetTickCount();
		if(aTick - mLastTick >= 50)
		{
			mLastTick = aTick;
			if(mRootContainer->WantTimer())
				mRootContainer->TimerEvent(50);
		}

		// Render the scene
		mRootContainer->Invalidate();
		if(mRootContainer->IsInvalid())
		{
			mRootContainer->PrePaint(mGraphics);
			mRootContainer->Paint(mGraphics);
		}

		// Present the backbuffer contents to the display
		mDevice->Present( NULL, NULL, NULL, NULL );
    }

	return 0;
}


