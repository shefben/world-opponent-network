#ifndef __WON_EVENTTYPES_H__
#define __WON_EVENTTYPES_H__

namespace WONAPI
{
	enum WindowEventType
	{
		WindowEventType_None,

		InputEvent_SizeChange,
		InputEvent_MouseMove,
		InputEvent_MouseDown,
		InputEvent_MouseDoubleClick,
		InputEvent_MouseDownCheckDoubleClick, // Have Window figure out whether this is a double click or not (operating systems that don't natively support double clicks can use this)
		InputEvent_MouseUp,
		InputEvent_MouseWheel,
		InputEvent_KeyDown,
		InputEvent_KeyUp,
		InputEvent_KeyChar,
		InputEvent_GotFocus,
		InputEvent_LostFocus,
		InputEvent_MouseExit,
		InputEvent_WindowClose,

		ComponentEvent_ButtonPressed,
		ComponentEvent_Scroll,
		ComponentEvent_TextLinkActivated,
		ComponentEvent_HyperLinkActivated,
		ComponentEvent_HyperLinkOver,
		ComponentEvent_InputReturn,
		ComponentEvent_InputTextChanged,
//		ComponentEvent_ScrollAreaChange,
		ComponentEvent_ListSelChanged,
		ComponentEvent_ListItemClicked,
		ComponentEvent_ListItemUpClicked,
		ComponentEvent_MenuItemSelected,
		ComponentEvent_TreeItemOpened,
		ComponentEvent_ComboSelChanged,
		ComponentEvent_TabBarTabSelected,
		ComponentEvent_TabCtrlTabSelected,
		ComponentEvent_EndPopup,
		ComponentEvent_BrowserStatusChange,
		ComponentEvent_BrowserLocationChange,
		ComponentEvent_BrowserButtonChange,
		ComponentEvent_SpinnerValueChanged,

		ComponentEvent_ContextStart	= 10000, 
		ComponentEvent_ContextEnd = 20000,

		Event_User_Start = 100000
	};
	
	enum ControlIdType
	{
		ControlId_LibMin	= 100000,
		ControlId_Ok,
		ControlId_Cancel,
		ControlId_TreeToggleOpen,
		ControlId_SplitterMin,
		ControlId_SplitterMax = ControlId_SplitterMin + 100,
		ControlId_UserMin	= 200000,
		ControlId_UserMin2	= 300000,
		ControlId_UserMin3	= 400000,
		ControlId_UserMin4	= 500000,
		ControlId_UserMin5	= 600000
	};

	enum MouseButton
	{
		MouseButton_Left			= 0,
		MouseButton_Middle			= 1,
		MouseButton_Right			= 2
	};

};

#endif