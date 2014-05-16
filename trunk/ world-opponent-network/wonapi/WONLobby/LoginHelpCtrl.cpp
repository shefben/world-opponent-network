#include "LoginHelpCtrl.h"

#include "WONGUI/BrowserComponent.h"
#include "WONGUI/GUIConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoginHelpCtrl::LoginHelpCtrl()
{
	mHaveLoadedHelp = false;
}	


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginHelpCtrl::Paint(Graphics &g)
{
	if(!mHaveLoadedHelp)
	{
		mBrowser->Go(ComponentConfig::GetResourceFile("help/loginhelp.html"));
		mBrowser->SetLaunchBrowserOnLink(true);
		mHaveLoadedHelp = true;
	}

	Container::Paint(g);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginHelpCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mBrowser,"Browser");
	}

}


