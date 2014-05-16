#ifndef __LOSTUSERNAMECTRL_H__
#define __LOSTUSERNAMECTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Button);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LostUserNameCtrl : public Container
{
protected:
	InputBoxPtr mEmailInput;
	ButtonPtr mRetrieveButton;

	void CheckInput();
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	LostUserNameCtrl();
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<LostUserNameCtrl> LostUserNameCtrlPtr;

};

#endif