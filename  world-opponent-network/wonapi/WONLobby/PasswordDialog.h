#ifndef __WON_PASSWORDDIALOG_H__
#define __WON_PASSWORDDIALOG_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Button);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PasswordDialog : public Dialog
{
protected:
	InputBoxPtr mPasswordInput;
	ButtonPtr mOkButton;
	ButtonPtr mCancelButton;

public:
	PasswordDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void Reset(); 
	const GUIString& GetPassword();
};
typedef SmartPtr<PasswordDialog> PasswordDialogPtr;

}; // namespace WONAPI

#endif