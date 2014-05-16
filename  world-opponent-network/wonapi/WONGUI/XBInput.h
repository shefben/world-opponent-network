

#ifndef __WON_XBINPUT_H__
#define __WON_XBINPUT_H__

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Window.h"
#include "XBGraphics.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XBInput 
{
	//-----------------------------------------------------------------------------
	// Name: struct XBGAMEPAD
	// Desc: structure for holding Gamepad data
	//-----------------------------------------------------------------------------
	struct XBGAMEPAD : public XINPUT_GAMEPAD
	{
		// Inherited members from XINPUT_GAMEPAD
		//
		// WORD    wButtons;
		// BYTE    bAnalogButtons[8];
		// SHORT   sThumbLX;
		// SHORT   sThumbLY;
		// SHORT   sThumbRX;
		// SHORT   sThumbRY;

		// Thumb stick values in range [-1,+1]
		FLOAT      fX1;
		FLOAT      fY1;
		FLOAT      fX2;
		FLOAT      fY2;
    
		// Buttons pressed since last poll
		WORD       wLastButtons;
		BOOL       bLastAnalogButtons[8];
		WORD       wPressedButtons;
		BOOL       bPressedAnalogButtons[8];

		// Device properties
		XINPUT_CAPABILITIES caps;
		HANDLE     hDevice;
	};

public:
	const float XBINPUT_DEADZONE;
	const float XBINPUT_BUTTONTHRESHOLD;

	// Devices are created on a global scale
	static XBGAMEPAD	mGamepad[2];
	static DWORD		mNumDevices;

public:
	static bool	mDeviceInit;
	static void InitDevices(DWORD numDevices);
	void GetInput();
	bool IsNewInput();


	XBInput();
	~XBInput();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __WON_XBINPUT_H__
