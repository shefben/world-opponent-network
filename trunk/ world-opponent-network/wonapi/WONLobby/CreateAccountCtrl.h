#ifndef __CREATEACCOUNTCTRL_H__
#define __CREATEACCOUNTCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CreateAccountCtrl : public Container
{
protected:
	InputBoxPtr mUserNameInput;
	InputBoxPtr mPasswordInput;
	InputBoxPtr mConfirmPasswordInput;
	InputBoxPtr mEmailInput;

	InputBoxPtr mMonthInput;
	InputBoxPtr mDayInput;
	InputBoxPtr mYearInput;

	ButtonPtr mNewsletterCheck;

	LabelPtr mUserNameDesc;
	LabelPtr mPasswordDesc;
	LabelPtr mConfirmPasswordDesc;
	LabelPtr mEmailDesc;
	LabelPtr mBirthdateDesc;

	ButtonPtr mCreateButton;

	bool CheckBirthdate(InputBox *theMonth, InputBox *theDay, InputBox *theYear);
	bool SetDesc(bool theCondition, Label *theLabel);
	void CheckInput();

	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	CreateAccountCtrl();
	virtual void InitComponent(ComponentInit &theInit);

	static bool CheckEmail(const std::string &theEmail);
};
typedef SmartPtr<CreateAccountCtrl> CreateAccountCtrlPtr;

} // namespace WONAPI

#endif