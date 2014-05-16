//----------------------------------------------------------------------------------
// PatchDetailsCtrl.h
//----------------------------------------------------------------------------------
#ifndef __PatchDetailsCtrl_H__
#define __PatchDetailsCtrl_H__

#include "WONAPI/WONCommon/AsyncOp.h"
#include "WONAPI/WONMisc/HTTPGetOp.h"
#include "WONAPI/WONMisc/FTPGetOp.h"
#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/Button.h"
#include "WizardCtrl.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// PatchDetailsCtrl.
//----------------------------------------------------------------------------------
class PatchDetailsCtrl : public WizardCtrl
{
	//------------------------------------------------------------------------------
	// PatchDetailsCompletion.
	//------------------------------------------------------------------------------
	class PatchDetailsCompletion : public WONAPI::CompletionBase<WONAPI::AsyncOpPtr>
	{
	private:
		typedef void(PatchDetailsCtrl::*Callback)(WONAPI::AsyncOp *theOp);
		Callback mCallback;

	public:
		PatchDetailsCompletion(Callback theCallback) : mCallback(theCallback) {}
		virtual void Complete(WONAPI::AsyncOpPtr theOp) 
		{
			if (PatchDetailsCtrl::m_pInstance)
				(PatchDetailsCtrl::m_pInstance->*mCallback)(theOp);
		}
	};

protected:
	TextAreaPtr  m_pInfoText;          // Main text presented to the user.
	ButtonPtr    m_pHelpButton;        // Help Button (may not be visible).
	ButtonPtr    m_pBackButton;        // Back Button.
	ButtonPtr    m_pNextButton;        // Next Button.
	bool         m_bDownloadStarted;   // Have we started a download?
	bool         m_bDownloadComplete;  // Has the download finished?
	bool         m_bDownloadSucceeded; // Did the download succeed?
	bool         m_bAborted;           // Has the user aborted the download.
	FTPGetOpPtr  m_pFtpGetFile;        // FTP Get Operation for the file.
	HTTPGetOpPtr m_pHttpGetFile;       // HTTP Get Operation for the file.
	std::string  m_sInfoFile;          // Temp File to download the info to.
	std::string  m_sURL;               // URL of the detail file to download.

	void EnableControls(void);

	void LoadInfoTextFromFile(const std::string& sFile);

	void HttpDownloadComplete(HTTPGetOp* pGetOp);
	void FtpDownloadComplete(FTPGetOp* pGetOp);
	void KillThreads(void);
	void StartDownload(void);

public:
	static PatchDetailsCtrl* m_pInstance; // Global instance of this dialog.

	PatchDetailsCtrl(void);
	~PatchDetailsCtrl(void);

	void Show(bool bShow = true);

	inline void FireBackButton(void)        { m_pBackButton->Activate(); }
	inline void FireNextButton(void)        { m_pNextButton->Activate(); }

	bool HandleHelpButton(ComponentEvent* pEvent);
	bool HandleBackButton(ComponentEvent* pEvent);
	bool HandleNextButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);

	virtual void InitComponent(ComponentInit &theInit);

	void HttpDownloadDoneCallback(AsyncOp* pOp);
	void FtpDownloadDoneCallback(AsyncOp* pOp);
};
typedef SmartPtr<PatchDetailsCtrl> PatchDetailsCtrlPtr;

};

#endif
