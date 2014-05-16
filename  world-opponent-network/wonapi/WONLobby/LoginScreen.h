#ifndef __WON_LOGINSCREEN_H__
#define __WON_LOGINSCREEN_H__

#include "WONGUI/SimpleComponent.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LoginScreen : public Container
{
protected:
	ScreenContainerPtr mRightPanel;

	ComponentPtr mMOTDCtrl;
	ComponentPtr mCreateAccountCtrl;
	ComponentPtr mLostUserNameCtrl;
	ComponentPtr mLostPasswordCtrl;
	ComponentPtr mLoginHelpCtrl;

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	LoginScreen();
	virtual void InitComponent(ComponentInit &theInit);

	void ShowMOTD();
	void ShowCreateAccountCtrl();
	void ShowLostPasswordCtrl();
	void ShowLostUsernameCtrl();
};
typedef SmartPtr<LoginScreen> LoginScreenPtr;

};

#endif