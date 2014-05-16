#ifndef __WON_GUITYPES_H__
#define __WON_GUITYPES_H__

#include "WONCommon/SmartPtr.h"
#include "Rectangle.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WON_PTR_FORWARD(Window);
WON_PTR_FORWARD(Component);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ComponentInitTypes
{
	ComponentInitType_Resource				= 1,
	ComponentInitType_ComponentConfig,

	ComponentInitType_User_Start = 100000
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ComponentInit
{
	int mType;

	ComponentInit(int theType) : mType(theType) { }
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum DialogFlags
{
	DialogFlag_InOwnWindow					= 0x0001,
	DialogFlag_Popup						= 0x0002,

	DialogFlag_StandardDialog				= 0x0000
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct DialogParams
{
	Window *mParent;
	Window *mDlgWindow;
	Component *mDialog;
	int mFlags;
	int mx,my;

	DialogParams(Component *theDialog, int theFlags, int x = -1, int y = -1);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum PopupFlags
{
	PopupFlag_EndOnLeftMouseDown			= 0x0001,
	PopupFlag_EndOnMiddleMouseDown			= 0x0002,
	PopupFlag_EndOnRightMouseDown			= 0x0004,
	PopupFlag_EndOnMouseMove				= 0x0008,
	PopupFlag_EndOnKeyDown					= 0x0010,
	PopupFlag_EndOnEscape					= 0x0020,
	PopupFlag_EndOnFocusChange				= 0x0040,
	PopupFlag_WantInput						= 0x0080,
	PopupFlag_IsModal						= 0x0100,
	PopupFlag_AdjustPosition				= 0x0200,
	PopupFlag_EatMouseDownOnEnd				= 0x0800,
	PopupFlag_Additional					= 0x1000,
	PopupFlag_FireEventOnEnd				= 0x2000,
	PopupFlag_DontWantMouse					= 0x4000,				

	PopupFlag_InOwnWindow				   = 0x10000,
	PopupFlag_NotInOwnWindow			   = 0x20000,

	PopupFlag_EndOnMouseDown				= 0x0007,
	PopupFlag_EndOnInput					= 0x001f,

	PopupFlag_StandardPopup					= 0x23e7,
	PopupFlag_StandardTip					= 0x6247
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct PopupParams
{
	Window *mParent;
	Component *mPopup;
	Component *mInputComponent;
	Component *mOwnerComponent;
	int mFlags;
	int mx,my;
	WONRectangle mAvoidRect;

	PopupParams(Component *thePopup, int theFlags, Component *theOwner, int x = -1, int y = -1);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum Direction
{
	Direction_Left,
	Direction_Right,
	Direction_Up,
	Direction_Down
};



}; // namespace WONAPI

#endif