#ifndef __WON_ACCOUNTLOGIC_H__
#define __WON_ACCOUNTLOGIC_H__

#include "LobbyLogic.h"

namespace WONAPI
{

WON_PTR_FORWARD(CreditsDialog);
WON_PTR_FORWARD(TOUDialog);

class CreateAccountEvent;
class GetLostPasswordEvent;
class GetLostUserNameEvent;
class LoginEvent;
class UpdateAccountEvent;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AccountLogic : public LobbyLogic
{
protected:
	std::wstring mLastLoginName;
	bool mNeedTOUOnCreate;


	CreditsDialogPtr mCreditsDialog;
	TOUDialogPtr mTOUDialog;

	void DoCredits();
	bool DoTermsOfUse();

	void DoCreateAccount(CreateAccountEvent *theEvent); 
	void DoGetLostPassword(GetLostPasswordEvent *theEvent); 
	void DoGetLostUserName(GetLostUserNameEvent *theEvent); 
	void DoLogin(LoginEvent *theEvent); 
	void DoQueryAccount(); 
	void DoUpdateAccount(UpdateAccountEvent *theEvent); 
	bool CheckVersionStatus();

	virtual ~AccountLogic();

public:
	AccountLogic();

	virtual void Init(ComponentConfig *theConfig);
	virtual void HandleControlEvent(int theControlId);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
};

} // namespace WONAPI

#endif