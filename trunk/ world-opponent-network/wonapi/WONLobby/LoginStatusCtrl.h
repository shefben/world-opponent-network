#ifndef __WON_LOGINSTATUSCTRL_H__
#define __WON_LOGINSTATUSCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LoginStatusCtrl : public Container
{
protected:
	LabelPtr mStatusLabel;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	LoginStatusCtrl();
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<LoginStatusCtrl> LoginStatusCtrlPtr;

}; // namespace WONAPI

#endif