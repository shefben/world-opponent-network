//----------------------------------------------------------------------------------
// MotdCtrl.h
//----------------------------------------------------------------------------------
#ifndef __MotdCtrl_H__
#define __MotdCtrl_H__

#include "WONAPI/WONCommon/AsyncOp.h"
#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/Button.h"
#include "WizardCtrl.h"
#include "PatchCore.h"

namespace WONAPI
{

WON_PTR_FORWARD(BrowserComponent);

//----------------------------------------------------------------------------------
// MotdCtrl.
//----------------------------------------------------------------------------------
class MotdCtrl : public WizardCtrl
{
	//------------------------------------------------------------------------------
	// MotDCtrlCallback.
	//------------------------------------------------------------------------------
	class MotDCtrlCallback : public PatchCoreCallback
	{
	private:
		typedef void(MotdCtrl::*Callback)(OperationInfo* pOpInfo);
		Callback m_Callback;

	public:
		MotDCtrlCallback(Callback theCallback) : m_Callback(theCallback) {}
		virtual void Complete(OperationInfo* pOpInfo) 
		{
			if (MotdCtrl::m_pInstance)
				(MotdCtrl::m_pInstance->*m_Callback)(pOpInfo);
		}
	};

protected:
	BrowserComponentPtr m_pInfoText;   // Main text presented to the user.
	ButtonPtr           m_pHelpButton; // Help Button (may not be visible).
	ButtonPtr           m_pBackButton; // Back Button.
	ButtonPtr           m_pNextButton; // Next Button.

	void DnsTestCallback(OperationInfo* pOpInfo);
	void MotdCallback(OperationInfo* pOpInfo);

public:
	static MotdCtrl* m_pInstance; // Global instance of this dialog.

	MotdCtrl(void);
	~MotdCtrl(void);

	void Show(bool bShow = true);

	inline void FireBackButton(void)        { m_pBackButton->Activate(); }
	inline void FireNextButton(void)        { m_pNextButton->Activate(); }

	bool HandleHelpButton(ComponentEvent* pEvent);
	bool HandleBackButton(ComponentEvent* pEvent);
	bool HandleNextButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<MotdCtrl> MotdCtrlPtr;

};

#endif
