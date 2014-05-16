#ifndef __WON_LoginHelpCtrl_H__
#define __WON_LoginHelpCtrl_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

WON_PTR_FORWARD(BrowserComponent);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LoginHelpCtrl : public Container
{
protected:
	BrowserComponentPtr mBrowser;
	bool mHaveLoadedHelp;

	virtual void Paint(Graphics &g);

public:
	LoginHelpCtrl();
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<LoginHelpCtrl> LoginHelpCtrlPtr;

};

#endif