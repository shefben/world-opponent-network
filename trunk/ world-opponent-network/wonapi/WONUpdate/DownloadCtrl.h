//----------------------------------------------------------------------------------
// DownloadCtrl.h
//----------------------------------------------------------------------------------
#ifndef __DownloadCtrl_H__
#define __DownloadCtrl_H__

#include "WONAPI/WONCommon/AsyncOp.h"
#include "WONAPI/WONMisc/HTTPGetOp.h"
#include "WONAPI/WONMisc/FTPGetOp.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/Button.h"
#include "WONGUI/ImageButton.h"
#include "WizardCtrl.h"
#include "ProgressBarComponent.h"
#include "LoginCtrl.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// DownloadCtrl.
//----------------------------------------------------------------------------------
class DownloadCtrl : public WizardCtrl
{
	//------------------------------------------------------------------------------
	// DownloadCompletion.
	//------------------------------------------------------------------------------
	class DownloadCompletion : public WONAPI::CompletionBase<WONAPI::AsyncOpPtr>
	{
	private:
		typedef void(DownloadCtrl::*Callback)(WONAPI::AsyncOp *theOp);
		Callback mCallback;

	public:
		DownloadCompletion(Callback theCallback) : mCallback(theCallback) {}
		virtual void Complete(WONAPI::AsyncOpPtr theOp) 
		{
			if (DownloadCtrl::m_pInstance)
				(DownloadCtrl::m_pInstance->*mCallback)(theOp);
		}
	};

protected:
	TextAreaPtr             m_pInfoText;                // Main text presented to the user.
	MSLabelPtr              m_pVisitHostLabel;          // Visit Host Prompt.
	ButtonPtr               m_pVisitHostButton;         // User wants to go to the host's web site.
	ProgressBarComponentPtr m_pProgressBar;             // Download Progress Bar.
	MSLabelPtr              m_pProgressStatusLabel;     // Progress status (percent complete).
	MSLabelPtr              m_pProgressLabel;			// Download Progress:
	MS3DFramePtr            m_pProgressFrame;			// Progress bar frame
	MSLabelPtr              m_pTimeLeftLabel;           // Estimated Time Remaining.
	ButtonPtr               m_pHelpButton;              // Help Button (may not be visible).
	ButtonPtr               m_pBackButton;              // Back Button.
	ButtonPtr               m_pNextButton;              // Next (Finsihed) Button.
	ImageButtonPtr          m_pVisitHostImageButton;    // Button to replace the Host button with (if downloaded).
	int                     m_tDownloading;             // How long have we been downloading (or trying to).
	bool                    m_bDownloadStarted;         // Have we started a download?
	bool                    m_bPatchDownloadComplete;   // Has the patch download finished?
	bool                    m_bPatchDownloadSucceeded;  // Did the download succeed?
	bool                    m_bBitmapDownloadComplete;  // Has the patch download finished?
	bool                    m_bBitmapDownloadSucceeded; // Did the download succeed?
	bool                    m_bAborted;                 // Has the user aborted the download.
	bool                    m_bShowingAbortDlg;         // Is the Abort dialog being shown?
	bool                    m_bShownTimeoutWarning;     // Have we shown the 'something may be wrong' warning (for this download)?
	bool                    m_bValidatingPatch;         // Are we validating the patch.
	bool                    m_bPatchIsValid;            // Did we successfully validate the patch?
	FTPGetOpPtr             m_pFtpGetPatch;             // FTP Get Operation for the patch itself.
	HTTPGetOpPtr            m_pHttpGetPatch;            // HTTP Get Operation for the patch itself.
	FTPGetOpPtr             m_pFtpGetBitmap;            // FTP Get Operation for the host bitmap.
	HTTPGetOpPtr            m_pHttpGetBitmap;           // HTTP Get Operation for the host bitmap.
	int                     m_nPrevDownloadedFtpBytes;  // How many bytes were previously downloaded?
	std::string             m_sHostBitmapFile;          // File the host's bitmap was downloaded to.

	LoginCtrlPtr                     m_pLoginCtrl;         // Login dialog control

	time_t mStartTime;
	int    mNumberCallbacks;

	void EnableControls(void);

	void UpdateProgressBar(int nDownloaded, int nTotalSize);
	void UpdateProgressBar(RecvBytesOp* pRcvOp);
	void ResetProgressBar(void);
	bool ValidatePatch(void);
	void SendReport(void);

	void NamePasswordDialog(FTPGetOp* pGetOp, const GUIString& theTitle, const GUIString& theText);

	void HttpPatchDownloadComplete(HTTPGetOp* pGetOp);
	void FtpPatchDownloadComplete(FTPGetOp* pGetOp);
	void HttpBitmapDownloadComplete(HTTPGetOp* pGetOp);
	void FtpBitmapDownloadComplete(FTPGetOp* pGetOp);

	void StartDownloads(void);
	void KillThreads(void);

public:
	static DownloadCtrl* m_pInstance; // Global instance of this dialog.

	DownloadCtrl(void);
	~DownloadCtrl(void);

	void Show(bool bShow = true);
	bool CanClose(void);

	void SetHostImage(const std::string& sFile);

	inline void FireBackButton(void)        { m_pBackButton->Activate(); }
	inline void FireNextButton(void)        { m_pNextButton->Activate(); }

	void HttpPatchDoneCallback(AsyncOp* pOp);
	void FtpPatchDoneCallback(AsyncOp* pOp);
	void HttpBitmapDoneCallback(AsyncOp* pOp);
	void FtpBitmapDoneCallback(AsyncOp* pOp);
	void PatchProgressCallback(AsyncOp* pOp);

	bool HandleVisitHostButton(ComponentEvent* pEvent);
	bool HandleHelpButton(ComponentEvent* pEvent);
	bool HandleBackButton(ComponentEvent* pEvent);
	bool HandleNextButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);
	virtual bool TimerEvent(int tDelta);

	virtual void InitComponent(ComponentInit &theInit);

	void SetLoginCtrl(LoginCtrlPtr pLoginCtrl) { m_pLoginCtrl = pLoginCtrl; }
};
typedef SmartPtr<DownloadCtrl> DownloadCtrlPtr;

};

#endif
