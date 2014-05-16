#ifndef __WON_AddFriendDIALOG_H__
#define __WON_AddFriendDIALOG_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(InputBox);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AddFriendDialog : public Dialog
{
protected:
	InputBoxPtr mNameInput;
	ButtonPtr mOkButton;
	ButtonPtr mCancelButton;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	void CheckInput();

public:
	AddFriendDialog();
	virtual void InitComponent(ComponentInit &theInit);
	void Reset();
	const GUIString& GetName();
};
typedef SmartPtr<AddFriendDialog> AddFriendDialogPtr;

}; // namespace WONAPI

#endif