#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <X11/keysym.h>
#include <X11/Xlib.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{
	
enum
{
	KEYCODE_UP			= XK_Up,
	KEYCODE_DOWN		= XK_Down,
	KEYCODE_LEFT		= XK_Left,
	KEYCODE_RIGHT		= XK_Right,
	KEYCODE_HOME		= XK_Home,
	KEYCODE_END			= XK_End,
	KEYCODE_PGDN		= XK_Next,
	KEYCODE_PGUP		= XK_Prior,

	KEYCODE_CONTROL		= XK_Control_L,
	KEYCODE_SHIFT		= XK_Shift_L,
	KEYCODE_ALT			= XK_Alt_L,

	KEYCODE_RETURN		= XK_Return,
	KEYCODE_BACK		= XK_BackSpace,
	KEYCODE_DELETE		= XK_Delete,
	KEYCODE_SPACE		= XK_space,
	KEYCODE_TAB			= XK_Tab,

	KEYCODE_ESCAPE		= XK_Escape
};

}; // namepsace WONAPI


#endif
