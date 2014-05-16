#ifndef __INITFAILEDCTRL_H__
#define __INITFAILEDCTRL_H__

#include "WONGUI/WONGUIConfig/ContainerConfig.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(Label);

class LoginInitStatusEvent;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class InitFailedCtrl : public Container
{
protected:
	LabelPtr mDescLabel;
	ButtonPtr mRetryButton;
	ButtonPtr mIgnoreButton;

	void HandleLoginInitStatus(LoginInitStatusEvent *theEvent);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	InitFailedCtrl();
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<InitFailedCtrl> InitFailedCtrlPtr;

};

#endif
