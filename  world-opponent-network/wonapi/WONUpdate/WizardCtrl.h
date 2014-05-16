//----------------------------------------------------------------------------------
// WizardCtrl.h
//
// Base Control for all of the Wizard screens.  The common functionality is 
// captured here and passed to the appropriate (active) control as needed.
//----------------------------------------------------------------------------------
#ifndef __WizardCtrl_H__
#define __WizardCtrl_H__

#include "WONGUI/GUICompat.h"
#include "WONGUI/Container.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// WizardCtrl
//----------------------------------------------------------------------------------
class WizardCtrl : public Container
{
public:
	WizardCtrl(void);

	virtual void FireBackButton(void) = 0;
	virtual void FireNextButton(void) = 0;
};
typedef SmartPtr<WizardCtrl> WizardCtrlPtr;

}; // namespace

#endif
