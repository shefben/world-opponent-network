#ifndef __DIRECTCONNECTDIALOG_H__
#define __DIRECTCONNECTDIALOG_H__


#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(InputBox);
class IPAddr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DirectConnectDialog : public Dialog
{
protected:
	InputBoxPtr mNameInput;
	InputBoxPtr mAddressInput;
	ButtonPtr mAddButton;
	ButtonPtr mCancelButton;
	
public:
	DirectConnectDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void Reset();

	const GUIString& GetName();
	const GUIString& GetAddress();
};
typedef SmartPtr<DirectConnectDialog> DirectConnectDialogPtr;

}; // namespace WONAPI

#endif