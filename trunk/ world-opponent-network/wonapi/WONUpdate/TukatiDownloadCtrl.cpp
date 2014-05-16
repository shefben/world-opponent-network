//----------------------------------------------------------------------------------
// TukatiDownloadCtrl.cpp
//----------------------------------------------------------------------------------
/*@@@
#include <assert.h>
#include "WONCommon/MiscUtil.h"
#include "WONMisc/ReportPatchStatusOp.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "TukatiDownloadCtrl.h"
#include "WONUpdateCtrl.h"
#include "AbortDlg.h"
#include "CustomInfo.h"
#include "PatchUtils.h"
#include "MessageDlg.h"
#include "DebugLog.h"
#include "PatchCore.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Constants.
//----------------------------------------------------------------------------------
const int CHUNCK_SIZE           = 10000;        // Number of bytes betwwen update ticks.
//const int UM_DELAYED_CLOSE    = WM_USER + 10;
//const int NO_DATA_TIMEOUT     = 30000;        // Number of milliseconds w/o data to assume a hang.

const GUIString sHoursKey       = "%HOURS%";
const GUIString sMinutesKey     = "%MINUTES%";
const GUIString sSecondsKey     = "%SECONDS%";
const GUIString sPercentDoneKey = "%PERCENT_DONE%";
const GUIString sReceivedKey    = "%RECEIVED%";
const GUIString sTotalSizeKey   = "%TOTAL_SIZE%";
const GUIString sQueuePosKey    = "%QUEUE_POS%";


//----------------------------------------------------------------------------------
// Static member variables.
//----------------------------------------------------------------------------------
TukatiDownloadCtrl* TukatiDownloadCtrl::m_pInstance = NULL;
CTukatiCallBack gTukatiCallback;


//----------------------------------------------------------------------------------
// DownloadError controls
//----------------------------------------------------------------------------------
class DownloadErrorEvent : public WindowEventBase
{
public:
	GUIString mErrorString;

	DownloadErrorEvent(GUIString theErrorString) : mErrorString(theErrorString) { }
	virtual void Deliver() { MessageBox(TukatiDownloadCtrl::m_pInstance->GetWindow(), mErrorString, Download_DownloadError_String, MD_OK); }
};

static void DownloadError(GUIString theErrorString)
{
	WindowManager *aMgr = WindowManager::GetDefaultWindowManager();
	if(aMgr!=NULL)
		aMgr->PostEvent(new DownloadErrorEvent(theErrorString));
}
	
//----------------------------------------------------------------------------------
// xxx controls
//----------------------------------------------------------------------------------
class ProgressEvent : public WindowEventBase
{
public:
	DWORD mBytesReceived;
	DWORD mBytesPerSec;

	ProgressEvent(DWORD theBytesRecieved, DWORD theBytesPerSec) : mBytesReceived(theBytesReceived), mBytesPerSec(theBytesPerSec) { }
	virtual void Deliver() { TukatiDownloadCtrl::m_pInstance->UpdateProgressBarDownload(mBytesReceived, mBytesPerSec); }
};

static void UpdateProgressBarDownload(DWORD theBytesRecieved, DWORD theBytesPerSec)
{
	WindowManager *aMgr = WindowManager::GetDefaultWindowManager();
	if(aMgr!=NULL)
		aMgr->PostEvent(new ProgressEvent(theBytesRecieved, theBytesPerSec));
}
	
void CTukatiCallBack::UpgradeProgress  (TUKATI_HANDLE handle, int percent, DWORD error)
{
	if(TukatiDownloadCtrl::m_pInstance)
	{
		//@@@ need code here
	}
}

//----------------------------------------------------------------------------------
// UpdateProgressBarQueued controls
//----------------------------------------------------------------------------------
class ProgresQueuedEvent : public WindowEventBase
{
public:
	int mPosition;
	int mSeconds;

	ProgresQueuedEvent(int theSeconds, int thePosition) : mSeconds(theSeconds), mPosition(thePosition) { }
	virtual void Deliver() { TukatiDownloadCtrl::m_pInstance->UpdateProgressBarQueued(mPosition, mSeconds); }
};

static void UpdateProgressBarQueued(int theSeconds, int thePosition)
{
	WindowManager *aMgr = WindowManager::GetDefaultWindowManager();
	if(aMgr!=NULL)
		aMgr->PostEvent(new ProgresQueuedEvent(theSeconds, thePosition));
}
	
void CTukatiCallBack::DownloadQueued (TUKATI_HANDLE handle, DWORD position, DWORD seconds)
{
	if(TukatiDownloadCtrl::m_pInstance->m_hDownloadHandle != handle || TukatiDownloadCtrl::m_pInstance->m_hDownloadHandle == INVALID_TUKATI_HANDLE)
		return;

	if(TukatiDownloadCtrl::m_pInstance)
		UpdateProgressBarQueued(position,seconds);
}

//----------------------------------------------------------------------------------
// UpdateProgressBarDownload controls
//----------------------------------------------------------------------------------
class ProgressUpdateEvent : public WindowEventBase
{
public:
	DWORD mBytesReceived;
	DWORD mBytesPerSec;

	ProgressUpdateEvent(DWORD theBytesReceived, DWORD theBytesPerSec) : mBytesReceived(theBytesReceived), mBytesPerSec(theBytesPerSec) { }
	virtual void Deliver() { TukatiDownloadCtrl::m_pInstance->UpdateProgressBarDownload(mBytesReceived, mBytesPerSec); }
};

static void UpdateProgressBarDownload(DWORD theBytesRecieved, DWORD theBytesPerSec)
{
	WindowManager *aMgr = WindowManager::GetDefaultWindowManager();
	if(aMgr!=NULL)
		aMgr->PostEvent(new ProgressUpdateEvent(theBytesRecieved, theBytesPerSec));
}
	
void CTukatiCallBack::DownloadStarted (TUKATI_HANDLE handle, DWORD fileByteSize)
{
	if(TukatiDownloadCtrl::m_pInstance)
	{
		if(TukatiDownloadCtrl::m_pInstance->GetPatchSize() != fileByteSize)
			DownloadError(Global_WrongSize_String);
		else
		{
			TukatiDownloadCtrl::m_pInstance->m_hDownloadHandle = handle;
			UpdateProgressBarDownload(0,0);
		}
	}
}

void CTukatiCallBack::DownloadProgress (TUKATI_HANDLE handle, DWORD bytesRecieved, DWORD bytesPerSec)
{
	if(TukatiDownloadCtrl::m_pInstance->m_hDownloadHandle != handle || TukatiDownloadCtrl::m_pInstance->m_hDownloadHandle == INVALID_TUKATI_HANDLE)
		return;

	if(TukatiDownloadCtrl::m_pInstance)
		UpdateProgressBarDownload(bytesRecieved, bytesPerSec);
}

//----------------------------------------------------------------------------------
// PatchDownloadComplete controls
//----------------------------------------------------------------------------------
class ProgressDoneEvent : public WindowEventBase
{
public:
	DWORD mError;

	ProgressDoneEvent(DWORD theError) : mError(theError) { }
	virtual void Deliver() { TukatiDownloadCtrl::m_pInstance->PatchDownloadComplete(mError); }
};

static void PatchDownloadComplete(DWORD theError)
{
	WindowManager *aMgr = WindowManager::GetDefaultWindowManager();
	if(aMgr!=NULL)
		aMgr->PostEvent(new ProgressDoneEvent(theError));
}
	
void CTukatiCallBack::DownloadDone (TUKATI_HANDLE handle, DWORD error)
{
	if(TukatiDownloadCtrl::m_pInstance->m_hDownloadHandle != handle || TukatiDownloadCtrl::m_pInstance->m_hDownloadHandle == INVALID_TUKATI_HANDLE)
		return;

	if(TukatiDownloadCtrl::m_pInstance)
	{
		TukatiDownloadCtrl::m_pInstance->m_hDownloadHandle = INVALID_TUKATI_HANDLE;
		PatchDownloadComplete(error);
	}
}

//----------------------------------------------------------------------------------
// ConfirmEndEvent.
//----------------------------------------------------------------------------------
class ConfirmEndEvent : public WindowEventBase
{
public:
//	CRCThreadDataPtr mCRCData;

	virtual void Deliver()
	{
//		if(mCRCData->m_bAborted)
//			return;

		WindowManager::GetDefaultWindowManager()->EndDialog(MDR_NO);
	}

//	ConfirmEndEvent(CRCThreadData *theData) : mCRCData(theData) { }
	ConfirmEndEvent(void) { }
};


//----------------------------------------------------------------------------------
// TukatiDownloadCtrl Constructor.
//----------------------------------------------------------------------------------
TukatiDownloadCtrl::TukatiDownloadCtrl(void)
	: WizardCtrl()
{
	m_pInstance = this;
	m_tDownloading = 0;
	m_bDownloadStarted = false;
	m_bPatchDownloadComplete = false;
	m_bPatchDownloadSucceeded = false;
	m_bAborted = false;
	m_bShownTimeoutWarning = false;
	m_bShowingAbortDlg = false;
	m_bValidatingPatch = false;
	m_bPatchIsValid = false;
	m_hDownloadHandle = INVALID_TUKATI_HANDLE;
}

//----------------------------------------------------------------------------------
// TukatiDownloadCtrl Destructor.
//----------------------------------------------------------------------------------
TukatiDownloadCtrl::~TukatiDownloadCtrl(void)
{
	m_pInstance = NULL;
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
	
		WONComponentConfig_Get(aConfig,m_pInfoText,"InformationText");
		WONComponentConfig_Get(aConfig,m_pHelpButton,"HelpButton");
		WONComponentConfig_Get(aConfig,m_pBackButton,"BackButton");
		WONComponentConfig_Get(aConfig,m_pNextButton,"NextButton");

		WONComponentConfig_Get(aConfig,m_pVisitHostButton,"VisitHostButton");
		WONComponentConfig_Get(aConfig,m_pProgressBar,"ProgressBar");
		WONComponentConfig_Get(aConfig,m_pProgressFrame,"ProgressFrame");
		WONComponentConfig_Get(aConfig,m_pProgressStatusLabel,"ProgressStatusLabel");
		WONComponentConfig_Get(aConfig,m_pProgressLabel,"ProgressLabel");
		WONComponentConfig_Get(aConfig,m_pTimeLeftLabel,"TimeLeftLabel");
		WONComponentConfig_Get(aConfig,m_pVisitHostImageButton,"VisitHostImageButton");
		WONComponentConfig_Get(aConfig,m_pSpeedPassButton,"SpeedPassButton");

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetWelcomeHelpCallback() != NULL);
	}
}

//----------------------------------------------------------------------------------
// EnableControls: Enable or disable controls as appropriate.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::EnableControls(void)
{
	m_pNextButton->Enable(m_bPatchDownloadComplete);

	if (m_bPatchDownloadComplete)
		m_pNextButton->RequestFocus();

	if (m_bPatchIsValid && IsVisible())
		FireNextButton();
}

//----------------------------------------------------------------------------------
// CanClose: Is it okay to close this window?  In other words, ask the user if they 
// really want to abort the download (if its still in progress).
//----------------------------------------------------------------------------------
bool TukatiDownloadCtrl::CanClose(void)
{
	if (m_bPatchDownloadComplete)
		return true;

	if (m_bValidatingPatch)
		return false;

	// The MessageBox does not block access to this call so make sure we don't trap 
	// ourselves.
	if (m_bShowingAbortDlg)
		return false;

	m_bShowingAbortDlg = true;

	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();
	int nResult = MessageBox(GetWindow(), 
							 pResMgr->GetFinishedString(Download_AbandonDownload_String), 
							 pResMgr->GetFinishedString(Download_Confirm_String), MD_YESNO);

	m_bShowingAbortDlg = false;

	// Did the download complete while we were waiting?
	if (nResult == MDR_NO && m_bPatchDownloadSucceeded)
	{
		ValidatePatch();
		EnableControls();
	}

	if (nResult == MDR_YES)
	{
		// Tag the patch as canceled.
		CPatchData* pPatch = GetCustomInfo()->GetSelectedPatch();
		pPatch->SetDownloadAborts(pPatch->GetDownloadAborts() + 1);
	}

	return nResult == MDR_YES;
}

//----------------------------------------------------------------------------------
// ResetProgressBar: Reset the progress bar (and related text) to the beginning of 
// the download.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::ResetProgressBar(void)
{
	CPatchData* pPatch = GetCustomInfo()->GetSelectedPatch();

	int nTotSize = pPatch ? pPatch->GetPatchSize() : 0; // Sanity check.

	UpdateProgressBarQueued(1, 1);
//	UpdateProgressBarDownload(0, 0);
}

//----------------------------------------------------------------------------------
// GetPatchSize: Get the size of the selected patch 
//----------------------------------------------------------------------------------
DWORD TukatiDownloadCtrl::GetPatchSize(void)
{
	CPatchData* pPatch = GetCustomInfo()->GetSelectedPatch();
	return pPatch->GetPatchSize();
}

//----------------------------------------------------------------------------------
// GetTimeString: Generate time remaining string .
//----------------------------------------------------------------------------------
GUIString TukatiDownloadCtrl::GetTimeString(int nTimeLeft)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	int nHours = nTimeLeft / (60 * 60);
	int nMinutes = (nTimeLeft / 60) % 60;
	int nSeconds = nTimeLeft % 60;

	GUIString sOutput;
	if (nHours > 1)
		sOutput = pResMgr->GetFinishedString(Download_TimeHoursMinutes_String);
	else if (nHours)
		sOutput = pResMgr->GetFinishedString(Download_TimeHourMinutes_String);
	else if (nMinutes > 1)
		sOutput = pResMgr->GetFinishedString(Download_TimeMinutesSeconds_String);
	else if (nMinutes)
		sOutput = pResMgr->GetFinishedString(Download_TimeMinuteSeconds_String);
	else
		sOutput = pResMgr->GetFinishedString(Download_TimeSeconds_String);

	TCHAR sTime[32];
	wsprintf(sTime, "%d", nHours);
	GUIString sHours = sTime;
	wsprintf(sTime, "%d", nMinutes);
	GUIString sMinutes = sTime;
	wsprintf(sTime, "%d", nSeconds);
	GUIString sSeconds = sTime;

	ReplaceSubString(sOutput, sHoursKey, sHours);
	ReplaceSubString(sOutput, sMinutesKey, sMinutes);
	ReplaceSubString(sOutput, sSecondsKey, sSeconds);

	return sOutput;
}

//----------------------------------------------------------------------------------
// UpdateProgressBarQueued: Update the status bar (and related text) to reflect the 
// supplied position in the queue.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::UpdateProgressBarQueued(DWORD nPosition, DWORD nSeconds)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	if (! m_bAborted)
	{
		// Time Remaining text.
		GUIString sOutput;
		if (nSeconds && nSeconds < (60 * 60 * 24)) // Make sure its less than a day.
			sOutput = GetTimeString(nSeconds);
		else
			sOutput = pResMgr->GetFinishedString(Tukati_Estimating_String);

		m_pTimeLeftLabel->SetText(sOutput);
		m_pTimeLeftLabel->Invalidate();

		// Download Progress text.
		TCHAR sNumber[32];
		sOutput = pResMgr->GetFinishedString(Tukati_QueuedPosition_String);
		wsprintf(sNumber, "%d", nPosition);
		ReplaceSubString(sOutput, sQueuePosKey, sNumber);

		m_pProgressStatusLabel->SetText(sOutput);
		m_pProgressStatusLabel->Invalidate();
	}
}

//----------------------------------------------------------------------------------
// UpdateProgressBarDownload: Update the status bar (and related text) to reflect the 
// supplied position of the download.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::UpdateProgressBarDownload(DWORD nBytesRcvd, DWORD nBytesPerSec)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();
	CPatchData* pPatch = GetCustomInfo()->GetSelectedPatch();

	if (nBytesRcvd > 0)
		RequestTimer(false);

	if (! m_bAborted)
	{
		DWORD nTotalSize = pPatch ? pPatch->GetPatchSize() : 1;

		// Update the Progress Bar.
		m_pProgressBar->SetRange(0, nTotalSize);
		m_pProgressBar->SetPosition(nBytesRcvd);

		// Time Remaining text.
		DWORD tElapsed = m_tDownloading;
		if (nTotalSize == 0)
			nTotalSize = nBytesRcvd; // Sanity Check (avoid a rare divide by zero).

		int nPercentDone = nBytesRcvd ? (int)((100.0 * (double)nBytesRcvd) / (double)nTotalSize): 0;

		if(nBytesPerSec == 0)
			nBytesPerSec = 1;

		DWORD nTimeLeft = ((nTotalSize - nBytesRcvd) / nBytesPerSec);

		GUIString sOutput;
		if (nTotalSize == nBytesRcvd) // Are we done?
			sOutput = "";
		else if (nTimeLeft && nTimeLeft < (60 * 60 * 24)) // Make sure its less than a day.
			sOutput = GetTimeString(nTimeLeft);
		else
			sOutput = pResMgr->GetFinishedString(Tukati_Estimating_String);

		m_pTimeLeftLabel->SetText(sOutput);
		m_pTimeLeftLabel->Invalidate();

		// Download Progress text.
		sOutput = pResMgr->GetFinishedString(Tukati_BytesTransferred_String);

		TCHAR sNumber[32];
		wsprintf(sNumber, "%d", nPercentDone);
		ReplaceSubString(sOutput, sPercentDoneKey, sNumber);
		ReplaceSubString(sOutput, sReceivedKey, NumToStrWithCommas(nBytesRcvd));
		ReplaceSubString(sOutput, sTotalSizeKey, NumToStrWithCommas(nTotalSize));

		m_pProgressStatusLabel->SetText(sOutput);
		m_pProgressStatusLabel->Invalidate();
	}
}

//----------------------------------------------------------------------------------
// PatchDownloadComplete: The Tukati transfer has completed for the patch.  Check 
// the status to see if it worked.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::PatchDownloadComplete(DWORD nError)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	// Turn off the timer.
	RequestTimer(false);

	CPatchData* pPatch = pCustInfo->GetSelectedPatch();

	if (nError == TUKATI_NO_ERROR)
	{
		if (pPatch)
			pCustInfo->SaveDefaultSelection(pPatch->GetHostName());

		m_bPatchDownloadSucceeded = true;

		if (! m_bAborted)
		{
			// Set the progress bar to 100%.
			int nLower, nUpper;
			m_pProgressBar->GetRange(nLower, nUpper);
			UpdateProgressBarDownload(nUpper, nUpper);
			ValidatePatch();
		}
	}
	else
	{
		if (! m_bAborted) // If the user aborted don't pop up an error telling them they aborted...
		{
			// Clear the file name (so no one tries to use it), but don't deleted it, 
			// we can resume the download later.
			pCustInfo->SetPatchFile("");

			GUIString sTitle = pResMgr->GetFinishedString(Download_DownloadError_String);
			GUIString sMsg;

			if (pPatch)
				pPatch->SetDownloadFailures(pPatch->GetDownloadFailures() + 1);

			switch (nError)
			{
				case WS_HTTP_InvalidHeader:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_InvalidHttpHeader_String), sTitle, MD_OK);
					break;
				default:
				{
					char aNumberStr[10];
					sMsg = pResMgr->GetFinishedString(Download_UnknownHttpError_String);
					ReplaceSubString(sMsg, "%ERROR%", ltoa(nError, aNumberStr, 10));
					MessageBox(GetWindow(), sMsg, sTitle, MD_OK);
				}
			} //lint !e788

			FireBackButton();
		}
	}

	m_bPatchDownloadComplete = true;
	SendReport();
	EnableControls();
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::Show(bool bShow)
{
	SetVisible(bShow);

	if (bShow)
	{
		m_bPatchIsValid = false;
		m_bShownTimeoutWarning = false;
//		m_nReturnResult = SUR_UNKNOWN;
		GetMainControl()->SetReturnResult(SUR_UNKNOWN);

		CustomInfo* pCustInfo = GetCustomInfo();
		ResourceManager* pResMgr = pCustInfo->GetResourceManager();

		// Some of the text changes dynamically based on patch selection, update it.

		// Informational (Main) text.
		GUIString sInfo = pResMgr->BuildInfoString(Tukati_Info1_String_Id);
		m_pInfoText->Clear();
		m_pInfoText->AddFormatedText(sInfo);
		m_pInfoText->SetVertOffset(0); // Scroll to the top.

		// Visit Host button.
		m_pVisitHostButton->SetText(pResMgr->GetFinishedString(Tukati_VisitHost_String));

		DWORD anError = Tukati_Init("../TukatiSDK/Bin"); //@@@ get dll directory from config

		if(Tukati_IsRedistributorInstalled() || m_isDoNotShowSpeedPass)
		if(Tukati_IsRedistributorInstalled())
		{
			if(m_pSpeedPassButton->IsVisible())
			{
				m_pSpeedPassButton->SetVisible(false);
				EnableLayout(1,true);
				RecalcLayout();
			}
		}
		else
		{
			if(!m_pSpeedPassButton->IsVisible())
			{
				m_pSpeedPassButton->SetVisible(true);
				EnableLayout(1,false);
				RecalcLayout();
			}
		}

		ResetProgressBar();

		// Make sure the Next button is the default button.
		m_pNextButton->RequestFocus();

		// Start the download clock.
		m_tDownloading = 0;
		m_bDownloadStarted = false;
		m_bPatchDownloadComplete = false;
		m_bPatchDownloadSucceeded = false;
		m_bAborted = false;
		RequestTimer(true);

		m_pVisitHostButton->SetVisible(true);
		m_pVisitHostImageButton->SetVisible(false);

		EnableControls();

		if(anError != TUKATI_NO_ERROR)
		{
			PatchDownloadComplete(anError);
			return;
		}

		// Check to see if we are using a previously downloaded patch.
		if (pCustInfo->GetPatchFile() != "")
		{
			m_bPatchDownloadComplete = true;

			int nLower, nUpper;
			m_pProgressBar->GetRange(nLower, nUpper);
			UpdateProgressBarDownload(nUpper, nUpper);

			ValidatePatch();  // Check it (we are paranoid).
			EnableControls();
		}
		else
			StartDownloads(); // Start the download itself.
	}
	else
	{
		// Turn off the timer.
		RequestTimer(false);

		// If the download is still in progress, kill it.
		KillThreads();
		if(m_hDownloadHandle != INVALID_TUKATI_HANDLE)
		{
			Tukati_UnInit();
			m_hDownloadHandle = INVALID_TUKATI_HANDLE;
		}
	}
}

//----------------------------------------------------------------------------------
// ValidatePatch: Check the patch size and checksum to see if they match.
//----------------------------------------------------------------------------------
bool TukatiDownloadCtrl::ValidatePatch(void)
{
	// If we are showing the abort confirmation dialog, then bail out (don't confuse the user).
	if (m_bShowingAbortDlg)
		return false;

	// Possible race condition:  The user clicks 'no' on the 'do you really want to 
	// abort' dialog, just as the download finishes.  This could cause us to enter 
	// this call two times.  I'm not certain this can happen, but better to be sure.
	if (m_bValidatingPatch)
		return false;

	m_bValidatingPatch = true;
	m_bPatchIsValid = false;

	PatchCore* pCore = GetMainControl()->GetPatchCore();
	int nPatchResult = SUR_UNKNOWN;

	// Did the user already pre-validate the patch (after being warned)?
	if (pCore->GetSkipValidation())
		nPatchResult = SUR_PATCH_READY;
	else
		nPatchResult = pCore->ValidateSelectedPatch(GetWindow());

	GetMainControl()->SetReturnResult(nPatchResult);

	// Do we have a green light?
	if (nPatchResult == SUR_PATCH_READY)
	{
		nPatchResult = pCore->ApplySelectedPatch(GetWindow());
		GetMainControl()->SetReturnResult(nPatchResult);

		// Did we patch the game?
		if (nPatchResult == SUR_PATCH_APPLIED) 
			m_bPatchIsValid = true;

		// Are we patching ourself (requires us to shut down).
		if (nPatchResult == SUR_PATCHING_SELF)
			m_bPatchIsValid = true;

		// Are we in a loop where we keep checking until the game is up to date?
		if (nPatchResult == SUR_START_OVER)
			m_bPatchIsValid = true;
	}

	m_bValidatingPatch = false;

	if (! m_bPatchIsValid)
		FireBackButton();

	return m_bPatchIsValid;
}

//----------------------------------------------------------------------------------
// SendReport: Send report on what happened with this patch. 
//		(for automatic pruning of poor patch locations) 
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::SendReport(void)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	CPatchData* pPatch = pCustInfo->GetSelectedPatch();
	PatchCore* pCore = GetMainControl()->GetPatchCore();

	ServerContext* pGamePatchServers = pCore->GetGamePatchServers();
	if (pGamePatchServers == NULL)
		return;

	AsyncSocketPtr aSocketP = new AsyncSocket(AsyncSocket::UDP);
	aSocketP->Bind(0);

	AddrList GamePatchServerAddrList;
	pGamePatchServers->CopyAddresses(GamePatchServerAddrList);
	for (AddrList::iterator anItr = GamePatchServerAddrList.begin(); anItr != GamePatchServerAddrList.end(); ++anItr)
	{
		ReportPatchStatusOpPtr pOp = new ReportPatchStatusOp(*anItr);
		pOp->SetProductName(pCustInfo->GetProductName());
		pOp->SetConfigName(pCustInfo->GetExtraConfig());
		pOp->SetFromVersion(pPatch->GetFromVersion());
		pOp->SetToVersion(pCustInfo->GetNewVersion()); 
		pOp->SetNetAddress("Tukati");
		pOp->SetUDPSocket(aSocketP);

		if (m_bAborted)
			pOp->SetPatchStatus(1); // cancel type
		else if (m_bPatchIsValid && m_bPatchDownloadSucceeded)
			pOp->SetPatchStatus(0); // success type
		else
			pOp->SetPatchStatus(2); // failed type

		pOp->RunBlock(0);
	}
}

//----------------------------------------------------------------------------------
// KillThreads: Blast the transfer threads.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::KillThreads(void)
{
	if (m_bDownloadStarted && ((! m_bPatchDownloadComplete)))
	{
		// Shut down the download threads.
		if (! m_bPatchDownloadComplete)
		{
			m_bAborted = true;

			SendReport();
		}
		if(m_hDownloadHandle != INVALID_TUKATI_HANDLE)
			Tukati_StopDownload(m_hDownloadHandle);
	}
}

//----------------------------------------------------------------------------------
// StartDownloads: Start the threads that will download the files.
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::StartDownloads(void)
{
	CustomInfo* pCustInfo = GetCustomInfo();

	CPatchData* pPatch = pCustInfo->GetSelectedPatch();
	if (pPatch)
	{
		#ifdef _DEBUG
			if (GetKeyState(VK_MENU) & GetKeyState(VK_CONTROL) & GetKeyState(VK_SHIFT) & 0x8000)
				pPatch->DebugBox(GetWindow());
		#endif

		GUIString sInfoString = pPatch->GetPatchUrl();
		sInfoString.erase(0, strlen("TUKATI://"));

		GUIString sPatchFile = pPatch->GetPatchUrl();
		int nPos = sPatchFile.findNoCase("hf");
		if(nPos != -1)
		{
			sPatchFile.erase(0, nPos+3);
			nPos = sPatchFile.find("&");
			if(nPos != -1)
				sPatchFile.erase(nPos);
		}

		nPos = sPatchFile.rFind('/');
		if (nPos == -1)
			nPos = sPatchFile.rFind('\\');
		if (nPos != -1)
			sPatchFile.erase(0, nPos + 1);
		else
			sPatchFile = TEXT("Update.exe");

		CreateDirectoryRecursive(pCustInfo->GetPatchFolder());
		GUIString sPatchExe = pCustInfo->GetPatchFolder();
		sPatchExe.append(sPatchFile);

		pCustInfo->SetPatchFile(sPatchExe);

		DWORD anError = 0;
		m_hDownloadHandle = Tukati_StartDownload(aStr1.c_str(), aStr2.c_str(), gTukatiCallback, anError);
		m_hDownloadHandle = Tukati_StartDownload(std::string(sInfoString).c_str(), std::string(sPatchExe).c_str(), gTukatiCallback, anError);
		if(m_hDownloadHandle != INVALID_TUKATI_HANDLE)
			m_bDownloadStarted = true;
		else
			PatchDownloadComplete(anError);
	}
}

//----------------------------------------------------------------------------------
// HandleVisitHostButton: Process a click on the Visit Host button.
//----------------------------------------------------------------------------------
bool TukatiDownloadCtrl::HandleVisitHostButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();
	CPatchData* pPatch = GetCustomInfo()->GetSelectedPatch();

	// Launch the user's browser (send them to the host's site).
	if (pPatch)
	{
		if (pPatch->GetHostUrl().length())
			Browse(pPatch->GetHostUrl());
		else
			MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_NoHostUrl_String), pResMgr->GetFinishedString(Global_Error_String), MD_OK);
	}

	return true;
}

//----------------------------------------------------------------------------------
// HandleHelpButton: Process a click on the Help button.
//----------------------------------------------------------------------------------
bool TukatiDownloadCtrl::HandleHelpButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	// Invoke the Help callback for this dialog.
	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowHelp();

	return true;
}

//----------------------------------------------------------------------------------
// HandleBackButton: Process a click on the Back button.
//----------------------------------------------------------------------------------
bool TukatiDownloadCtrl::HandleBackButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	// If we are in the midst of a download, make sure the user really wants to 
	// quit (ask for confirmation).
	if (CanClose())
	{
		WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
		assert(pUpdateControl);
		pUpdateControl->ShowPreviousScreen();
	}

	return true;
}

//----------------------------------------------------------------------------------
// HandleNextButton: Process a click on the Next button.
//----------------------------------------------------------------------------------
bool TukatiDownloadCtrl::HandleNextButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowNextScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleInstallSpeedPassButton: Process a click on the Next button.
//----------------------------------------------------------------------------------
bool TukatiDownloadCtrl::HandleInstallSpeedPassButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	TukatiSpeedPassInstallCtrlPtr aSpeedPassP = pUpdateControl->GetTukatiSpeedPassInstallCtrl();
	assert(aSpeedPassP.get());
	if(aSpeedPassP->DoDialog() == ControlId_Ok)
	{
		//@@@ handle install
	}

	return true;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void TukatiDownloadCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent))           ||
		(pEvent->mComponent == m_pBackButton && HandleBackButton(pEvent))           ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent))           ||
		(pEvent->mComponent == m_pVisitHostButton && HandleVisitHostButton(pEvent)) ||
		(pEvent->mComponent == m_pVisitHostImageButton && HandleVisitHostButton(pEvent)) ||
		(pEvent->mComponent == m_pSpeedPassButton && HandleInstallSpeedPassButton(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}

//----------------------------------------------------------------------------------
// TimerEvent: Handle a timer event.
//----------------------------------------------------------------------------------
bool TukatiDownloadCtrl::TimerEvent(int tDelta)
{
	Container::TimerEvent(tDelta);
	m_tDownloading += tDelta;

	if (! m_bShownTimeoutWarning)
	{
		CustomInfo* pCustInfo = GetCustomInfo();
		ResourceManager* pResMgr = pCustInfo->GetResourceManager();

		if (m_tDownloading > pCustInfo->GetPatchTimeout())
		{
			m_bShownTimeoutWarning = true;
			MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_NoData_String), pResMgr->GetFinishedString(Global_Warning_String), MD_OK);
		}
	}
	return true;
}


//----------------------------------------------------------------------------------
// TukatiSpeedPassInstallCtrl Constructor.
//----------------------------------------------------------------------------------
TukatiSpeedPassInstallCtrl::TukatiSpeedPassInstallCtrl(void)
{
}

//----------------------------------------------------------------------------------
// TukatiSpeedPassInstallCtrl Destructor.
//----------------------------------------------------------------------------------
TukatiSpeedPassInstallCtrl::~TukatiSpeedPassInstallCtrl(void)
{
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void TukatiSpeedPassInstallCtrl::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,m_pInfoText,"InformationText");
		WONComponentConfig_Get(aConfig,m_pInstallButton,"InstallButton");
		WONComponentConfig_Get(aConfig,m_pCancelButton,"CancelButton");
	}
}

//----------------------------------------------------------------------------------
// DoDialog: Display this dialog as a modal dialog.
//----------------------------------------------------------------------------------
int TukatiSpeedPassInstallCtrl::DoDialog(Window *pParent)
{
	PlatformWindowPtr pWindow = new PlatformWindow;
	pWindow->SetParent(pParent);
	pWindow->SetTitle(GetCustomInfo()->GetResourceManager()->GetFinishedString(Tukati_SpeedPassTitle_String));
	pWindow->SetCreateFlags(CreateWindow_SizeSpecClientArea | CreateWindow_NotSizeable);

	WONRectangle aRect;
	if (pParent != NULL)
		pParent->GetScreenPos(aRect);
	else
		pWindow->GetWindowManager()->GetScreenRect(aRect);

	// Center it on top of its parent (or the desktop).
	pWindow->Create(WONRectangle(aRect.Left() + (aRect.Width() - Width()) / 2,
								 aRect.Top() + (aRect.Height() - Height()) / 2,
								 Width(), Height()));

	pWindow->AddComponent(this);
	m_pInstallButton->RequestFocus();

	int nResult = pWindow->DoDialog(this);
	pWindow->Close();

	return nResult;
}

//----------------------------------------------------------------------------------
// HandleCloseButton: Close the dialog box.
//----------------------------------------------------------------------------------
bool TukatiSpeedPassInstallCtrl::HandleCloseButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	GetWindow()->EndDialog();

	return true;
}

//----------------------------------------------------------------------------------
// HandleCloseButton: Close the dialog box.
//----------------------------------------------------------------------------------
bool TukatiSpeedPassInstallCtrl::HandleInstallButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	GetWindow()->EndDialog();

	return true;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void TukatiSpeedPassInstallCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if (pEvent->mComponent == m_pCancelButton && HandleCloseButton(pEvent) ||
		pEvent->mComponent == m_pInstallButton && HandleInstallButton(pEvent))
		return;

	MSDialog::HandleComponentEvent(pEvent);
}
*/