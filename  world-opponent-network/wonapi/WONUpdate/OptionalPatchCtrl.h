//----------------------------------------------------------------------------------
// OptionalPatchCtrl.h
//----------------------------------------------------------------------------------
#ifndef __OptionalPatchCtrl_H__
#define __OptionalPatchCtrl_H__

#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/Button.h"
#include "WizardCtrl.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// OptionalPatchCtrl.
//----------------------------------------------------------------------------------
class OptionalPatchCtrl : public WizardCtrl
{
protected:
	TextAreaPtr m_pInfoText;           // Main text presented to the user.
	ButtonPtr   m_pPatchDetailsButton; // View Patch Details Button.
	ButtonPtr   m_pHelpButton;         // Help Button (may not be visible).
	ButtonPtr   m_pBackButton;         // Back Button.
	ButtonPtr   m_pNextButton;         // Next Button.

public:
	OptionalPatchCtrl(void);
	~OptionalPatchCtrl(void);

	void Show(bool bShow = true);

	inline void FireBackButton(void)        { m_pBackButton->Activate(); }
	inline void FireNextButton(void)        { m_pNextButton->Activate(); }

	bool HandlePatchDetailsButton(ComponentEvent* pEvent);
	bool HandleHelpButton(ComponentEvent* pEvent);
	bool HandleBackButton(ComponentEvent* pEvent);
	bool HandleNextButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<OptionalPatchCtrl> OptionalPatchCtrlPtr;

};

#endif
