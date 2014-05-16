#ifndef __WON_LOBBYCONTAINER_H__
#define __WON_LOBBYCONTAINER_H__

#include "WONGUI/SimpleComponent.h"

namespace WONAPI
{

WON_PTR_FORWARD(RootScreen);
WON_PTR_FORWARD(LoginScreen);


enum LobbyGameType;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyContainer : public ScreenContainer
{
protected:
	RootScreenPtr mInternetScreen;
	RootScreenPtr mLanScreen;
	LoginScreenPtr mLoginScreen;

	virtual ~LobbyContainer();

public:
	LobbyContainer();
	virtual void InitComponent(ComponentInit &theInit);

	LoginScreen* GetLoginScreen() { return mLoginScreen; }
	RootScreen* GetInternetScreen() { return mInternetScreen; }
	RootScreen* GetLanScreen() { return mLanScreen; }

	void ShowInternetScreen();
	void ShowLanScreen();
	void ShowLoginScreen();

	bool OnInternetScreen();
	bool OnLanScreen();
	bool OnLoginScreen();

	LobbyGameType GetGameScreenType();
};
typedef SmartPtr<LobbyContainer> LobbyContainerPtr;

} // namespace WONAPI

#endif
