#ifndef __WON_LOBBYSCREEN_H__
#define __WON_LOBBYSCREEN_H__

namespace WONAPI
{

enum LobbyGameType;
class Container;
class ColorScheme;
class Window;
class WindowManager;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyScreen
{
public:
	static Container* GetLobbyContainer();
	static ColorScheme* GetColorScheme();
	static Window* GetWindow();
	static WindowManager* GetWindowManager();
	static Container* GetCurScreen();

	static void ShowWindow(bool show);
	static bool IsWindowShowing();

	static void ShowInternetScreen();
	static void ShowLanScreen();
	static void ShowLoginScreen();

	static bool OnInternetScreen();
	static bool OnLanScreen();
	static bool OnLoginScreen();

	static void ShowLobbyTab();

	static void ShowMOTD();
	static void ShowCreateAccountCtrl();
	static void ShowLostPasswordCtrl();
	static void ShowLostUsernameCtrl();

	static LobbyGameType GetGameScreenType();
};

} // namespace WONAPI

#endif