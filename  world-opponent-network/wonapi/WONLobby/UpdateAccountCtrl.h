#ifndef __UPDATEACCOUNTCTRL_H__
#define __UPDATEACCOUNTCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Label);

class QueryAccountResultEvent;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UpdateAccountCtrl : public Container
{
protected:
	InputBoxPtr mUserNameInput;
	InputBoxPtr mOldPasswordInput;
	InputBoxPtr mPasswordInput;
	InputBoxPtr mConfirmPasswordInput;
	InputBoxPtr mEmailInput;

	ButtonPtr mNewsletterCheck;

	LabelPtr mUserNameDesc;
	LabelPtr mOldPasswordDesc;
	LabelPtr mPasswordDesc;
	LabelPtr mConfirmPasswordDesc;
	LabelPtr mEmailDesc;

	ButtonPtr mQueryButton;
	ButtonPtr mUpdateButton;

	GUIString mActualOldPassword;

	static bool SetDesc(bool theCondition, Label *theLabel);
	void CheckInput();

	void HandleEnteredScreen();
	void HandleQueryAccountResult(QueryAccountResultEvent *theEvent);

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);

	virtual ~UpdateAccountCtrl();

public:
	UpdateAccountCtrl();
};
typedef SmartPtr<UpdateAccountCtrl> UpdateAccountCtrlPtr;

};

#endif