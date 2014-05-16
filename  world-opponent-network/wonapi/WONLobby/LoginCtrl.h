#ifndef __LOGINCTRL_H__
#define __LOGINCTRL_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{
WON_PTR_FORWARD(ComboBox);
WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Button);
class LoginInitStatusEvent;
class SetLoginFieldsEvent;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LoginCtrl : public Dialog
{
protected:
	ComboBoxPtr mUserNameCombo;
	InputBoxPtr mPasswordInput;
	ButtonPtr mRememberPasswordCheck;
	ButtonPtr mLoginButton;
	bool mInitComplete;

	void HandleLoginInitStatus(LoginInitStatusEvent *theEvent);
	void HandleSetLoginFields(SetLoginFieldsEvent *theEvent);
	void SetLoginNames();
	void CheckFillPassword();
	void DoRememberPassword();
	void CheckEnableLoginButton();

	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	LoginCtrl();
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<LoginCtrl> LoginCtrlPtr;

};

#endif