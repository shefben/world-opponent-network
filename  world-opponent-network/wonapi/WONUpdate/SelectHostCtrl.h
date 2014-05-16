//----------------------------------------------------------------------------------
// SelectHostCtrl.h
//----------------------------------------------------------------------------------
#ifndef __SelectHostCtrl_H__
#define __SelectHostCtrl_H__

#include "WONMisc/GetVersionOp.h"
#include "WONMisc/FindPatchOp.h"
#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/Button.h"
#include "WizardCtrl.h"
#include "PatchCore.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// SelectHostCtrl.
//----------------------------------------------------------------------------------
class SelectHostCtrl : public WizardCtrl
{
	//------------------------------------------------------------------------------
	// SelectHostCtrlCallback.
	//------------------------------------------------------------------------------
	class SelectHostCtrlCallback : public PatchCoreCallback
	{
	private:
		typedef void(SelectHostCtrl::*Callback)(OperationInfo* pOpInfo);
		Callback m_Callback;

	public:
		SelectHostCtrlCallback(Callback theCallback) : m_Callback(theCallback) {}
		virtual void Complete(OperationInfo* pOpInfo) 
		{
			if (SelectHostCtrl::m_pInstance)
				(SelectHostCtrl::m_pInstance->*m_Callback)(pOpInfo);
		}
	};

protected:
	TextAreaPtr      m_pInfoText;                // Main text presented to the user.
	MultiListAreaPtr m_pHostList;                // List of servers hosting the update.
	ButtonPtr        m_pConfigProxyButton;       // Configure Proxy Button.
	ButtonPtr        m_pAlreadyHaveUpdateButton; // The user thinks the update is already on their disk.
	ButtonPtr        m_pHelpButton;              // Help Button (may not be visible).
	ButtonPtr        m_pBackButton;              // Back Button.
	ButtonPtr        m_pNextButton;              // Next Button.
//	int              m_nGetVerOpResult;          // Translated results of the operation.
//	int              m_nFindPatchOpResult;       // Translated results of the operation.
	bool             m_bMixedPatches;            // Are some patch direct download and do some require host visitation?
	GUIString        m_sLocalPatch;              // Patch selected from local storage (previously downloaded).

	void EnableControls(void);
	void UpdatePatchList(void);
	void UpdateInfoText(void);

	void SelfVersionCheckCallback(OperationInfo* pOpInfo);
	void GameVersionCheckCallback(OperationInfo* pOpInfo);

public:
	static SelectHostCtrl* m_pInstance; // Global instance of this dialog.

	SelectHostCtrl(void);
	~SelectHostCtrl(void);

	void Show(bool bShow = true);

	inline void FireBackButton(void)        { m_pBackButton->Activate(); }
	inline void FireNextButton(void)        { m_pNextButton->Activate(); }

	bool HandleAlreadyHaveUpdateButton(ComponentEvent* pEvent);
	bool HandleConfigProxyButton(ComponentEvent* pEvent);
	bool HandleHelpButton(ComponentEvent* pEvent);
	bool HandleBackButton(ComponentEvent* pEvent);
	bool HandleNextButton(ComponentEvent* pEvent);
	bool HandleHostList(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<SelectHostCtrl> SelectHostCtrlPtr;

};

#endif
