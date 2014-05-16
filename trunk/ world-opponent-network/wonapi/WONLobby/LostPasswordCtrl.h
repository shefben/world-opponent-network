#ifndef __LOSTPASSWORDCTRL_H__
#define __LOSTPASSWORDCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(ComboBox);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LostPasswordCtrl : public Container
{
protected:
	ComboBoxPtr mUserNameCombo;
	ButtonPtr mRetrieveButton;

	void CheckInput();
	void SetLoginNames();
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	LostPasswordCtrl();
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<LostPasswordCtrl> LostPasswordCtrlPtr;

};

#endif