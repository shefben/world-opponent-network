#ifndef __WON_GUICOMPAT_H__
#define __WON_GUICOMPAT_H__

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if defined(_XBOX)
//#include "XBWindow.h"
//namespace WONAPI
//{
//	typedef XBWindow PlatformWindow;
//}
#include "SWindow.h"
namespace WONAPI
{
	typedef SWindow PlatformWindow;
	typedef WindowManager PlatformWindowManager;
}
#define WONGUI_ENTRY_POINT int main()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#elif defined(WIN32)
#include "MSWindow.h"
namespace WONAPI
{
	typedef MSWindow PlatformWindow;
	typedef MSWindowManager PlatformWindowManager;
};
#define WONGUI_ENTRY_POINT int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#elif defined(_LINUX)
#include "XWindow.h"
namespace WONAPI
{
	typedef XWindow PlatformWindow;
	typedef XWindowManager PlatformWindowManager;
}
#define WONGUI_ENTRY_POINT int main()

#endif

namespace WONAPI
{
	typedef Window WindowCompat; // Window conflicts with Window defined in X.  Use this instead.
	typedef Cursor CursorCompat;
	typedef Font   FontCompat;

	typedef SmartPtr<PlatformWindow> PlatformWindowPtr;
	typedef SmartPtr<PlatformWindowManager> PlatformWindowManagerPtr;
}

#endif