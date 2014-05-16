//----------------------------------------------------------------------------------
// WelcomeCtrl.h
//----------------------------------------------------------------------------------
#ifndef __WelcomeCtrl_H__
#define __WelcomeCtrl_H__

#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"
#include "WizardCtrl.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// WelcomeCtrl.
//----------------------------------------------------------------------------------
class WelcomeCtrl : public WizardCtrl
{
protected:
	TextAreaPtr m_pInfoText;          // Main text presented to the user.
	ButtonPtr   m_pConfigProxyButton; // Configure Proxy Button.
	ButtonPtr   m_pHelpButton;        // Help Button (may not be visible).
	ButtonPtr   m_pBackButton;        // Back (Cancel) Button.
	ButtonPtr   m_pNextButton;        // Next (Continue) Button.

public:
	WelcomeCtrl(void);
	~WelcomeCtrl(void);

	void Show(bool bShow = true);

	inline void FireBackButton(void)        { m_pBackButton->Activate(); }
	inline void FireNextButton(void)        { m_pNextButton->Activate(); }

	bool HandleConfigProxyButton(ComponentEvent* pEvent);
	bool HandleHelpButton(ComponentEvent* pEvent);
	bool HandleBackButton(ComponentEvent* pEvent);
	bool HandleNextButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<WelcomeCtrl> WelcomeCtrlPtr;

};

#endif
