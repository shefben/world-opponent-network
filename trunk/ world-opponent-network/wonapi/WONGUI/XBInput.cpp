#include "XBGraphics.h"
#include "XBInput.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
using namespace WONAPI;

XBInput::XBGAMEPAD		XBInput::mGamepad[2];
unsigned long			XBInput::mNumDevices = 0;
bool					XBInput::mDeviceInit = false;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBInput::XBInput()
	: XBINPUT_DEADZONE(0.25),
	  XBINPUT_BUTTONTHRESHOLD(1)
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBInput::~XBInput()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBInput::InitDevices(DWORD numDevices)
{
	// Calling XInitDevices more than once will result in 
	// program termination
	if (mDeviceInit)
		return;

	// Initialize XBox input devices 
	XInitDevices(0,NULL);

    // Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );

    // Open the devices
	mNumDevices =  (numDevices <= XGetPortCount() ? numDevices : XGetPortCount());

    for( DWORD i=0; i < mNumDevices; i++ )
    {
        ZeroMemory( &mGamepad[i], sizeof(XBGAMEPAD) );
        if( dwDeviceMask & (1<<i) ) 
        {
            // Get a handle to the device
            mGamepad[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                                XDEVICE_NO_SLOT, NULL );

            // Store capabilites of the device
            XInputGetCapabilities( mGamepad[i].hDevice, &mGamepad[i].caps );
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBInput::GetInput()
{
    // Get status about gamepad insertions and removals. Note that, in order to
    // not miss devices, we will check for removed device BEFORE checking for
    // insertions
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );

    // Loop through all gamepads
    for( DWORD i=0; i < mNumDevices; i++ )
    {
        // Handle removed devices.
        if( dwRemovals & (1<<i) ) 
        {
            XInputClose( mGamepad[i].hDevice );
            mGamepad[i].hDevice = NULL;
        }

        // Handle inserted devices
        if( dwInsertions & (1<<i) ) 
        {
            mGamepad[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                               XDEVICE_NO_SLOT, NULL );
            XInputGetCapabilities(mGamepad[i].hDevice, &mGamepad[i].caps);
        }

        // If we have a valid device, poll it's state and track button changes
        if( mGamepad[i].hDevice )
        {
            // Read the input state
            XINPUT_STATE xiState;
            XInputGetState( mGamepad[i].hDevice, &xiState );

            // Copy gamepad to local structure
            mGamepad[i].wButtons          = xiState.Gamepad.wButtons;
            mGamepad[i].bAnalogButtons[0] = xiState.Gamepad.bAnalogButtons[0];
            mGamepad[i].bAnalogButtons[1] = xiState.Gamepad.bAnalogButtons[1];
            mGamepad[i].bAnalogButtons[2] = xiState.Gamepad.bAnalogButtons[2];
            mGamepad[i].bAnalogButtons[3] = xiState.Gamepad.bAnalogButtons[3];
            mGamepad[i].bAnalogButtons[4] = xiState.Gamepad.bAnalogButtons[4];
            mGamepad[i].bAnalogButtons[5] = xiState.Gamepad.bAnalogButtons[5];
            mGamepad[i].bAnalogButtons[6] = xiState.Gamepad.bAnalogButtons[6];
            mGamepad[i].bAnalogButtons[7] = xiState.Gamepad.bAnalogButtons[7];
            mGamepad[i].sThumbLX          = xiState.Gamepad.sThumbLX;
            mGamepad[i].sThumbLY          = xiState.Gamepad.sThumbLY;
            mGamepad[i].sThumbRX          = xiState.Gamepad.sThumbRX;
            mGamepad[i].sThumbRY          = xiState.Gamepad.sThumbRY;

            // Put Xbox device input for the gamepad into our custom format
            mGamepad[i].fX1 = (mGamepad[i].sThumbLX+0.5f)/32767.5f;
            if( fabsf(mGamepad[i].fX1) < XBINPUT_DEADZONE )
                mGamepad[i].fX1 = 0.0f;

            mGamepad[i].fY1 = (mGamepad[i].sThumbLY+0.5f)/32767.5f;
            if( fabsf(mGamepad[i].fY1) < XBINPUT_DEADZONE )
                mGamepad[i].fY1 = 0.0f;

            mGamepad[i].fX2 = (mGamepad[i].sThumbRX+0.5f)/32767.5f;
            if( fabsf(mGamepad[i].fX2) < XBINPUT_DEADZONE )
                mGamepad[i].fX2 = 0.0f;

            mGamepad[i].fY2 = (mGamepad[i].sThumbRY+0.5f)/32767.5f;
            if( fabsf(mGamepad[i].fY2) < XBINPUT_DEADZONE ) 
                mGamepad[i].fY2 = 0.0f;

            // Get the boolean buttons that have been pressed since the last
            // call. Each button is represented by one bit.
            mGamepad[i].wPressedButtons = ( mGamepad[i].wLastButtons ^ mGamepad[i].wButtons ) & mGamepad[i].wButtons;
            mGamepad[i].wLastButtons    = mGamepad[i].wButtons;

            // Get the analog buttons that have been pressed since the last
            // call. Here, we considered an analog button pressed (a boolean 
            // condition) if that value is >= the threshold.
            for( DWORD b=0; b<8; b++ )
            {
                // Turn the 8-bit polled value into a boolean value
                BOOL bPressed = ( mGamepad[i].bAnalogButtons[b] >= XBINPUT_BUTTONTHRESHOLD );

                if( bPressed )
                    mGamepad[i].bPressedAnalogButtons[b] = !mGamepad[i].bLastAnalogButtons[b];
                else
                    mGamepad[i].bPressedAnalogButtons[b] = FALSE;
                
                // Store the current state for the next time
                mGamepad[i].bLastAnalogButtons[b] = bPressed;
            }
        }
    }

}