#ifndef __CREATEROOMDIALOG_H__
#define __CREATEROOMDIALOG_H__


#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CreateRoomDialog : public Dialog
{
protected:
	InputBoxPtr mRoomNameInput;
	InputBoxPtr mPasswordInput;
	LabelPtr mPasswordLabel;
	ButtonPtr mPasswordCheck;
	ButtonPtr mCreateButton;
	
public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	void CheckInput();
	void UpdatePasswordInput();

public:
	CreateRoomDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void Reset(bool clearAll = false);

	const GUIString& GetRoomName();
	const GUIString& GetPassword();
};
typedef SmartPtr<CreateRoomDialog> CreateRoomDialogPtr;

}; // namespace WONAPI

#endif