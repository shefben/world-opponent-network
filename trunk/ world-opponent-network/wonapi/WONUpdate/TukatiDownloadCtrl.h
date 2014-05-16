//----------------------------------------------------------------------------------
// TukatiDownloadCtrl.h
//----------------------------------------------------------------------------------
#ifndef __TukatiDownloadCtrl_H__
#define __TukatiDownloadCtrl_H__
/*@@@
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/Button.h"
#include "WONGUI/ImageButton.h"
#include "WizardCtrl.h"
#include "ProgressBarComponent.h"

#include "TukatiAPI.h"
#include "TukatiError.h"

namespace WONAPI
{

class CTukatiCallBack : public TukatiCallBacks
{
public:
	CTukatiCallBack() {}
	virtual ~CTukatiCallBack() {}

	void UpgradeProgress  (TUKATI_HANDLE handle, int percent, DWORD error);
	void DownloadStarted (TUKATI_HANDLE handle, DWORD fileByteSize);
	void DownloadQueued (TUKATI_HANDLE handle, DWORD position, DWORD seconds);
	void DownloadProgress (TUKATI_HANDLE handle, DWORD bytesRecieved, DWORD bytesPerSec);
	void DownloadDone (TUKATI_HANDLE handle, DWORD error);
};


//----------------------------------------------------------------------------------
// DownloadCtrl.
//----------------------------------------------------------------------------------
class TukatiDownloadCtrl : public WizardCtrl
{
protected:
	TextAreaPtr             m_pInfoText;                // Main text presented to the user.
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
	ButtonPtr               m_pSpeedPassButton;			// Upgrade to SpeedPass Button.

	int                     m_tDownloading;             // How long have we been downloading (or trying to).
	bool                    m_bDownloadStarted;         // Have we started a download?
	bool                    m_bPatchDownloadComplete;   // Has the patch download finished?
	bool                    m_bPatchDownloadSucceeded;  // Did the download succeed?
	bool                    m_bAborted;                 // Has the user aborted the download.
	bool                    m_bShowingAbortDlg;         // Is the Abort dialog being shown?
	bool                    m_bShownTimeoutWarning;     // Have we shown the 'something may be wrong' warning (for this download)?
	bool                    m_bValidatingPatch;         // Are we validating the patch.
	bool                    m_bPatchIsValid;            // Did we successfully validate the patch?

	void EnableControls(void);

	GUIString GetTimeString(int nTimeLeft);

	void ResetProgressBar(void);
	bool ValidatePatch(void);
	void SendReport(void);

	void StartDownloads(void);
	void KillThreads(void);

public:
	static TukatiDownloadCtrl* m_pInstance; // Global instance of this dialog.

	TukatiDownloadCtrl(void);
	~TukatiDownloadCtrl(void);

	void Show(bool bShow = true);
	bool CanClose(void);

	DWORD GetPatchSize(void);
	void UpdateProgressBarQueued(DWORD nPosition, DWORD nSeconds);
	void UpdateProgressBarDownload(DWORD nBytesRcvd, DWORD nBytesPerSec);
	void PatchDownloadComplete(DWORD nError);
	TUKATI_HANDLE m_hDownloadHandle;          // Handle to the file being downloaded

	inline void FireBackButton(void)        { m_pBackButton->Activate(); }
	inline void FireNextButton(void)        { m_pNextButton->Activate(); }

	bool HandleVisitHostButton(ComponentEvent* pEvent);
	bool HandleHelpButton(ComponentEvent* pEvent);
	bool HandleBackButton(ComponentEvent* pEvent);
	bool HandleNextButton(ComponentEvent* pEvent);
	bool HandleInstallSpeedPassButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);
	virtual bool TimerEvent(int tDelta);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<TukatiDownloadCtrl> TukatiDownloadCtrlPtr;

//----------------------------------------------------------------------------------
// TukatiSpeedPassInstallCtrl.
//----------------------------------------------------------------------------------
class TukatiSpeedPassInstallCtrl : public MSDialog
{
protected:
	TextAreaPtr m_pInfoText;         // Main text presented to the user.
	ButtonPtr   m_pInstallButton;    // Install Button.
	ButtonPtr   m_pCancelButton;     // Cancel Button.

public:
	TukatiSpeedPassInstallCtrl(void);
	~TukatiSpeedPassInstallCtrl(void);

	int DoDialog(Window *pParent = NULL);

	bool HandleCloseButton(ComponentEvent* pEvent);
	bool HandleInstallButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<TukatiSpeedPassInstallCtrl> TukatiSpeedPassInstallCtrlPtr;

};
*/
#endif
