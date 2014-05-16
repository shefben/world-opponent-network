#ifndef __WON_LANNAMEDIALOG_H__
#define __WON_LANNAMEDIALOG_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(InputBox);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LanNameDialog : public Dialog
{
protected:
	InputBoxPtr mNameInput;
	ButtonPtr mOkButton;
	ButtonPtr mCancelButton;

	virtual ~LanNameDialog();

public:
	LanNameDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void Reset();
	const GUIString& GetName();
};
typedef SmartPtr<LanNameDialog> LanNameDialogPtr;

}; // namespace WONAPI

#endif