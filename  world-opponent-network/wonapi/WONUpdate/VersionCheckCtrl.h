//----------------------------------------------------------------------------------
// VersionCheckCtrl.h
//----------------------------------------------------------------------------------
#ifndef __VersionCheckCtrl_H__
#define __VersionCheckCtrl_H__

#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/Button.h"
#include "WizardCtrl.h"
#include "PatchCore.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// VersionCheckCtrl.
//----------------------------------------------------------------------------------
class VersionCheckCtrl : public WizardCtrl
{
	//------------------------------------------------------------------------------
	// VersionCheckCtrlCallback.
	//------------------------------------------------------------------------------
	class VersionCheckCtrlCallback : public PatchCoreCallback
	{
	private:
		typedef void(VersionCheckCtrl::*Callback)(OperationInfo* pOpInfo);
		Callback m_Callback;

	public:
		VersionCheckCtrlCallback(Callback theCallback) : m_Callback(theCallback) {}
		virtual void Complete(OperationInfo* pOpInfo) 
		{
			if (VersionCheckCtrl::m_pInstance)
				(VersionCheckCtrl::m_pInstance->*m_Callback)(pOpInfo);
		}
	};

protected:
	TextAreaPtr   m_pInfoText;           // Main text presented to the user.
	MSLabelPtr    m_pGeneralStatusText;  // Feedback text ('please wait', 'error', 'finished', etc.).
	MSLabelPtr    m_pBackText;           // Text informing the user they can go back safely.
	ButtonPtr     m_pHelpButton;         // Help Button (may not be visible).
	ButtonPtr     m_pBackButton;         // Back Button.
	ButtonPtr     m_pNextButton;         // Next Button.
	int           m_tLastBlink;          // When did the status text last toggle on or off?
	bool          m_bShowBackMsg;        // Should we show the 'ok to go back' msg?
	bool          m_bEnableNextButton;   // Ok to enable the 'Next' button?

	void DetermineControlStates(void);
	void EnableControls(void);
	void UpdateInfoText(void);
	void UpdateControls(void);

	void DnsTestCallback(OperationInfo* pOpInfo);
	void MotdCallback(OperationInfo* pOpInfo);
	void ServerLookupCallback(OperationInfo* pOpInfo);
	void SelfVerCheckCallback(OperationInfo* pOpInfo);
	void GameVerCheckCallback(OperationInfo* pOpInfo);

public:
	static VersionCheckCtrl* m_pInstance; // Global instance of this dialog.

	VersionCheckCtrl(void);
	~VersionCheckCtrl(void);

	void Show(bool bShow = true);

	inline void FireBackButton(void)        { m_pBackButton->Activate(); }
	inline void FireNextButton(void)        { m_pNextButton->Activate(); }

	bool HandlePatchDetailsButton(ComponentEvent* pEvent);
	bool HandleHelpButton(ComponentEvent* pEvent);
	bool HandleBackButton(ComponentEvent* pEvent);
	bool HandleNextButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);
	virtual bool TimerEvent(int tDelta);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<VersionCheckCtrl> VersionCheckCtrlPtr;

};

#endif
