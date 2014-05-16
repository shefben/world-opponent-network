#ifndef __WON_KEYBOARD_H__
#define __WON_KEYBOARD_H__

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum
{
	KEYMOD_SHIFT			= 0x0001,
	KEYMOD_ALT			= 0x0002,
	KEYMOD_CTRL			= 0x0004
};


}; // namespace WONAPI

#if defined(WIN32_NOT_XBOX)
#include "MSKeyboard.h"

#elif defined(_LINUX)
#include "XKeyboard.h"

#else

enum
{
	KEYCODE_UP,
	KEYCODE_DOWN,
	KEYCODE_LEFT,
	KEYCODE_RIGHT,
	KEYCODE_HOME,
	KEYCODE_END,
	KEYCODE_PGDN,
	KEYCODE_PGUP,

	KEYCODE_CONTROL,
	KEYCODE_SHIFT,
	KEYCODE_ALT,

	KEYCODE_RETURN,
	KEYCODE_BACK,
	KEYCODE_DELETE,
	KEYCODE_SPACE,
	KEYCODE_TAB,

	KEYCODE_ESCAPE
};

#endif

#endif
