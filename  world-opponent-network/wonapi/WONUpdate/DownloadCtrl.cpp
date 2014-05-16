//----------------------------------------------------------------------------------
// DownloadCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONCommon/MiscUtil.h"
#include "WONMisc/ReportPatchStatusOp.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "DownloadCtrl.h"
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


//----------------------------------------------------------------------------------
// Static member variables.
//----------------------------------------------------------------------------------
DownloadCtrl* DownloadCtrl::m_pInstance = NULL;


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
// DownloadCtrl Constructor.
//----------------------------------------------------------------------------------
DownloadCtrl::DownloadCtrl(void)
	: WizardCtrl()
{
	m_pInstance = this;
	m_tDownloading = 0;
	m_bDownloadStarted = false;
	m_bPatchDownloadComplete = false;
	m_bPatchDownloadSucceeded = false;
	m_bBitmapDownloadComplete = false;
	m_bBitmapDownloadSucceeded = false;
	m_bAborted = false;
	m_bShownTimeoutWarning = false;
	m_bShowingAbortDlg = false;
	m_bValidatingPatch = false;
	m_bPatchIsValid = false;
}

//----------------------------------------------------------------------------------
// DownloadCtrl Destructor.
//----------------------------------------------------------------------------------
DownloadCtrl::~DownloadCtrl(void)
{
	m_pInstance = NULL;
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void DownloadCtrl::InitComponent(ComponentInit &theInit)
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

		WONComponentConfig_Get(aConfig,m_pVisitHostLabel,"VisitHostLabel");
		WONComponentConfig_Get(aConfig,m_pVisitHostButton,"VisitHostButton");
		WONComponentConfig_Get(aConfig,m_pProgressBar,"ProgressBar");
		WONComponentConfig_Get(aConfig,m_pProgressFrame,"ProgressFrame");
		WONComponentConfig_Get(aConfig,m_pProgressStatusLabel,"ProgressStatusLabel");
		WONComponentConfig_Get(aConfig,m_pProgressLabel,"ProgressLabel");
		WONComponentConfig_Get(aConfig,m_pTimeLeftLabel,"TimeLeftLabel");
		WONComponentConfig_Get(aConfig,m_pVisitHostImageButton,"VisitHostImageButton");

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetWelcomeHelpCallback() != NULL);
	}
}

//----------------------------------------------------------------------------------
// EnableControls: Enable or disable controls as appropriate.
//----------------------------------------------------------------------------------
void DownloadCtrl::EnableControls(void)
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
bool DownloadCtrl::CanClose(void)
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
void DownloadCtrl::ResetProgressBar(void)
{
	CPatchData* pPatch = GetCustomInfo()->GetSelectedPatch();

	int nTotSize = pPatch ? pPatch->GetPatchSize() : 0; // Sanity check.

	UpdateProgressBar(0, nTotSize);
}

//----------------------------------------------------------------------------------
// UpdateProgressBar: Update the status bar (and related text) to reflect the 
// supplied position of the download.
//----------------------------------------------------------------------------------
void DownloadCtrl::UpdateProgressBar(int nBytesRcvd, int nTotalSize)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	if (nBytesRcvd > 0)
		RequestTimer(false);

	if (! m_bAborted)
	{
		// Update the Progress Bar.
		m_pProgressBar->SetRange(0, nTotalSize);
		m_pProgressBar->SetPosition(nBytesRcvd);

		// Time Remaining text.
		DWORD tElapsed = m_tDownloading;
		if (nTotalSize == 0)
			nTotalSize = nBytesRcvd; // Sanity Check (avoid a rare divide by zero).
		int nPercentDone = nBytesRcvd ? (int)((100.0 * (double)nBytesRcvd) / (double)nTotalSize): 0;

		int aTimeElapsed = time(NULL) - mStartTime;
		if(aTimeElapsed < 0)
			aTimeElapsed = 0;

		mNumberCallbacks++;

		int nBytesLeft = nTotalSize - nBytesRcvd;
		float fSecsPerByte = (float)aTimeElapsed / (float)nBytesRcvd;
		int nTimeLeft = (int)((float)nBytesLeft * fSecsPerByte);
		if(nTimeLeft < 1)
			nTimeLeft = 1;
			
		GUIString sOutput;
		if (nTotalSize == nBytesRcvd) // Are we done?
			sOutput = "";
		else if (mNumberCallbacks >= 3 && nTimeLeft && nTimeLeft < (60 * 60 * 24)) // Make sure its less than a day.
		{
			int nHours = nTimeLeft / (60 * 60);
			int nMinutes = (nTimeLeft / 60) % 60;
			int nSeconds = nTimeLeft % 60;

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
		}
		else
			sOutput = pResMgr->GetFinishedString(Download_Estimating_String);

		m_pTimeLeftLabel->SetText(sOutput);
		m_pTimeLeftLabel->Invalidate();

		// Download Progress text.
		sOutput = pResMgr->GetFinishedString(Download_BytesTransferred_String);

		TCHAR sPercent[32];
		wsprintf(sPercent, "%d", nPercentDone);
		ReplaceSubString(sOutput, sPercentDoneKey, sPercent);
		ReplaceSubString(sOutput, sReceivedKey, NumToStrWithCommas(nBytesRcvd));
		ReplaceSubString(sOutput, sTotalSizeKey, NumToStrWithCommas(nTotalSize));

		m_pProgressStatusLabel->SetText(sOutput);
		m_pProgressStatusLabel->Invalidate();
	}
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void DownloadCtrl::Show(bool bShow)
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
		GUIString sInfo = pResMgr->BuildInfoString(Download_Info1_String_Id);
		m_pInfoText->Clear();
		m_pInfoText->AddFormatedText(sInfo);
		m_pInfoText->SetVertOffset(0); // Scroll to the top.

		// Visit Host prompt.
		m_pVisitHostLabel->SetText(pResMgr->GetFinishedString(Download_Host_String));
		m_pVisitHostLabel->SetDesiredSize();

		// Visit Host button.
		m_pVisitHostButton->SetText(pResMgr->GetFinishedString(Download_VisitHost_String));

		ResetProgressBar();

		// Make sure the Next button is the default button.
		m_pNextButton->RequestFocus();

		// Start the download clock.
		m_tDownloading = 0;
		m_bDownloadStarted = false;
		m_bPatchDownloadComplete = false;
		m_bPatchDownloadSucceeded = false;
		m_bBitmapDownloadComplete = false;
		m_bBitmapDownloadSucceeded = false;
		m_bAborted = false;
		RequestTimer(true);

		m_pVisitHostButton->SetVisible(true);
		m_pVisitHostImageButton->SetVisible(false);

		EnableControls();

		// Check to see if we are using a previously downloaded patch.
		if (pCustInfo->GetPatchFile() != "")
		{
			m_bPatchDownloadComplete = true;

			int nLower, nUpper;
			m_pProgressBar->GetRange(nLower, nUpper);
			UpdateProgressBar(nUpper, nUpper);

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
	}
}

//----------------------------------------------------------------------------------
// SetHostImage: Load the Host's image from the downloaded file.
//----------------------------------------------------------------------------------
void DownloadCtrl::SetHostImage(const std::string& sFile)
{
	NativeImagePtr pHostImg = GetWindow()->DecodeImage(sFile.c_str());

	if (pHostImg.get() != NULL && pHostImg->GetWidth() <= Global_HOST_AD_WIDTH_Int && pHostImg->GetWidth() > 0 &&
		pHostImg->GetHeight() <= Global_HOST_AD_HEIGHT_Int && pHostImg->GetHeight() > 0)
	{
		m_pVisitHostImageButton->SetImage(ImageButtonType_Normal, pHostImg);

		m_pVisitHostButton->SetVisible(false);
		m_pVisitHostImageButton->SetVisible(true);
		Invalidate();
	}
}

//----------------------------------------------------------------------------------
// UpdateProgressBar: We have received some bytes, so update the progress bar.
//----------------------------------------------------------------------------------
void DownloadCtrl::UpdateProgressBar(RecvBytesOp* pRcvOp)
{
	CPatchData* pPatch = GetCustomInfo()->GetSelectedPatch();

//	DWORD nTotSize = pPatch ? pPatch->GetPatchSize() : pRcvOp->GetNumBytes();

	DWORD nTotSize = 0;
	if (m_pFtpGetPatch.get())
	{
		// FTP does not know how many bytes are coming (pRcvOp->GetNumBytes() is useless).
		nTotSize = static_cast<DWORD>(pPatch->GetPatchSize()) - m_nPrevDownloadedFtpBytes;
	}
	else
	{
		// HTTP should use pRcvOp->GetNumBytes() instead (already adjusted for previously downloaded portion).
		nTotSize = pRcvOp->GetNumBytes();
	}

	UpdateProgressBar(pRcvOp->GetCurRecv(), nTotSize);
}

//----------------------------------------------------------------------------------
// ValidatePatch: Check the patch size and checksum to see if they match.
//----------------------------------------------------------------------------------
bool DownloadCtrl::ValidatePatch(void)
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
void DownloadCtrl::SendReport(void)
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
		pOp->SetNetAddress(pPatch->GetPatchUrl());
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
// HttpPatchDownloadComplete: The http transfer has completed for the patch.  Check 
// the status to see if it worked.
//----------------------------------------------------------------------------------
void DownloadCtrl::HttpPatchDownloadComplete(HTTPGetOp* pGetOp)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	// Turn off the timer.
	RequestTimer(false);

	WONStatus aStatus = pGetOp->GetStatus();
	CPatchData* pPatch = pCustInfo->GetSelectedPatch();

	if (aStatus == WS_Success)
	{
		if (pPatch)
			pCustInfo->SaveDefaultSelection(pPatch->GetHostName());
		m_bPatchDownloadSucceeded = true;

		if (! m_bAborted)
		{
			// Set the progress bar to 100%.
			int nLower, nUpper;
			m_pProgressBar->GetRange(nLower, nUpper);
			UpdateProgressBar(nUpper, nUpper);
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

			switch (aStatus)
			{
				case WS_HTTP_StatusError:
					switch (pGetOp->GetHTTPStatus())
					{
						case 404:
							MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_Http404Error_String), sTitle, MD_OK);
							break;
						case 421:
							MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_Http421Error_String), sTitle, MD_OK);
							break;
						default:
							sMsg = pResMgr->GetFinishedString(Download_HttpError_String);
							ReplaceSubInt(sMsg, "%ERROR%", pGetOp->GetHTTPStatus());
							MessageBox(GetWindow(), sMsg, sTitle, MD_OK);
					} //lint !e788
					break;
				case WS_HTTP_InvalidHeader:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_InvalidHttpHeader_String), sTitle, MD_OK);
					break;
				case WS_HTTP_InvalidRedirect:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_InvalidHttpRedirect_String), sTitle, MD_OK);
					break;
				case WS_HTTP_TooManyRedirects:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_TooManyHttpRedirects_String), sTitle, MD_OK);
					break;
				case WS_WSAECONNREFUSED:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_ConnectionRefused_String), sTitle, MD_OK);
					break;
				case WS_WSAEAFNOSUPPORT:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_UnableToFindHost_String), sTitle, MD_OK);
					break;
				default:
					sMsg = pResMgr->GetFinishedString(Download_UnknownHttpError_String);
					ReplaceSubString(sMsg, "%ERROR%", WONStatusToString(aStatus));
					MessageBox(GetWindow(), sMsg, sTitle, MD_OK);
			} //lint !e788

			FireBackButton();
		}
	}

	m_bPatchDownloadComplete = true;
	SendReport();
	EnableControls();
}

//----------------------------------------------------------------------------------
// NamePasswordDialog: The ftp transfer has failed for the patch.  Get a UserName 
// and Password and try again.
//----------------------------------------------------------------------------------
void DownloadCtrl::NamePasswordDialog(FTPGetOp* pGetOp, const GUIString& theTitle, const GUIString& theText)
{
	if(m_pLoginCtrl->DoDialog(theTitle, theText) == ControlId_Ok)
	{
		pGetOp->SetUserName(m_pLoginCtrl->GetUserName());
		pGetOp->SetPassword(m_pLoginCtrl->GetPassword());
		pGetOp->RunAsync(static_cast<ULONG>(OP_TIMEOUT_INFINITE));
	}
}

//----------------------------------------------------------------------------------
// FtpPatchDownloadComplete: The ftp transfer has completed for the patch.  Check 
// the status to see if it worked.
//----------------------------------------------------------------------------------
void DownloadCtrl::FtpPatchDownloadComplete(FTPGetOp* pGetOp)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	// Turn off the timer.
	RequestTimer(false);

	WONStatus aStatus = pGetOp->GetStatus();
	CPatchData* pPatch = pCustInfo->GetSelectedPatch();

	if (aStatus == WS_Success)
	{
		if (pPatch)
			pCustInfo->SaveDefaultSelection(pPatch->GetHostName());
		m_bPatchDownloadSucceeded = true;

		if (! m_bAborted)
		{
			// Set the progress bar to 100%.
			int nLower, nUpper;
			m_pProgressBar->GetRange(nLower, nUpper);
			UpdateProgressBar(nUpper, nUpper);
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

			switch (aStatus)
			{
				case WS_FTP_StatusError:
					switch (pGetOp->GetFTPStatus())
					{
						case 500:
							NamePasswordDialog(pGetOp, sTitle, pResMgr->GetFinishedString(Download_InvalidFtpUser_String));
							return;
						case 530:
							NamePasswordDialog(pGetOp, sTitle, pResMgr->GetFinishedString(Download_InvalidFtpPassword_String));
							return;
						case 550:
							MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_InvalidFtpFile_String), sTitle, MD_OK);
							break;
						default:
							sMsg = pResMgr->GetFinishedString(Download_FtpError_String);
							ReplaceSubInt(sMsg, "%ERROR%", pGetOp->GetFTPStatus());
							MessageBox(GetWindow(), sMsg, sTitle, MD_OK);
					} //lint !e788
					break;
				case WS_FTP_InvalidResponse:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_InvalidFtpResp_String), sTitle, MD_OK);
					break;
				case WS_FTP_InvalidPasvResponse:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_InvalidFtpPasvResp_String), sTitle, MD_OK);
					break;
				case WS_WSAECONNREFUSED:
					MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_ConnectionRefused_String), sTitle, MD_OK);
					break;
				default:
					sMsg = pResMgr->GetFinishedString(Download_UnknownFtpError_String);
					ReplaceSubString(sMsg, "%ERROR%", WONStatusToString(aStatus));
					MessageBox(GetWindow(), sMsg, sTitle, MD_OK);
			} //lint !e788

			FireBackButton();
		}
	}

	m_bPatchDownloadComplete = true;
	SendReport();
	EnableControls();
}

//----------------------------------------------------------------------------------
// HttpBitmapDownloadComplete: The http transfer has completed for the bitmap.  
// Check the status to see if it worked.
//----------------------------------------------------------------------------------
void DownloadCtrl::HttpBitmapDownloadComplete(HTTPGetOp* pGetOp)
{
	WONStatus aStatus = pGetOp->GetStatus();

	switch (aStatus)
	{
		case WS_Success:
			SetHostImage(m_sHostBitmapFile);
			break;

		case WS_HTTP_StatusError:
			// To tell the truth, we don't really care if we don't receive the bitmap.
			// We could dump pGetOp->GetHTTPStatus() to a log file.
			break;

		//case WS_HTTP_InvalidHeader:
		//case WS_HTTP_InvalidRedirect:
		//case WS_HTTP_TooManyRedirects:
		default:
			// To tell the truth, we don't really care if we don't receive the bitmap.
			// We could dump WONStatusToString(aStatus) to a log file.
			break;
	} //lint !e788

	m_bBitmapDownloadComplete = true;
}

//----------------------------------------------------------------------------------
// FtpBitmapDownloadComplete: The ftp transfer has completed for the bitmap.  Check 
// the status to see if it worked.
//----------------------------------------------------------------------------------
void DownloadCtrl::FtpBitmapDownloadComplete(FTPGetOp* pGetOp)
{
	WONStatus aStatus = pGetOp->GetStatus();

	switch (aStatus)
	{
		case WS_Success:
			SetHostImage(m_sHostBitmapFile);
			break;

		case WS_FTP_StatusError:
			// To tell the truth, we don't really care if we don't receive the bitmap.
			// We could dump pGetOp->GetFTPStatus() to a log file.
			break;

		//case WS_FTP_InvalidResponse:
		//case WS_FTP_StatusError:
		//case WS_FTP_InvalidPasvResponse:
		default:
			// To tell the truth, we don't really care if we don't receive the bitmap.
			// We could dump WONStatusToString(aStatus) to a log file.
			break;
	} //lint !e788

	m_bBitmapDownloadComplete = true;
}

//----------------------------------------------------------------------------------
// HttpPatchDoneCallback: Download has completed (sucessfuly or otherwise).
//----------------------------------------------------------------------------------
void DownloadCtrl::HttpPatchDoneCallback(AsyncOp* pOp)
{
	HTTPGetOp* pGetOp = dynamic_cast<HTTPGetOp*>(pOp);

	if (pGetOp->Killed())
		return;

	HttpPatchDownloadComplete(pGetOp);

} //lint !e1746

//----------------------------------------------------------------------------------
// FtpPatchDoneCallback: Download has completed (sucessfuly or otherwise).
//----------------------------------------------------------------------------------
void DownloadCtrl::FtpPatchDoneCallback(AsyncOp* pOp)
{
	FTPGetOp* pGetOp = dynamic_cast<FTPGetOp*>(pOp);

	if (pGetOp->Killed())
		return;

	FtpPatchDownloadComplete(pGetOp);

} //lint !e1746

//----------------------------------------------------------------------------------
// HttpBitmapDoneCallback: Download has completed (sucessfuly or otherwise).
//----------------------------------------------------------------------------------
void DownloadCtrl::HttpBitmapDoneCallback(AsyncOp* pOp)
{
	HTTPGetOp* pGetOp = dynamic_cast<HTTPGetOp*>(pOp);

	if (pGetOp->Killed())
		return;

	HttpBitmapDownloadComplete(pGetOp);

} //lint !e1746

//----------------------------------------------------------------------------------
// FtpBitmapDoneCallback: Download has completed (sucessfuly or otherwise).
//----------------------------------------------------------------------------------
void DownloadCtrl::FtpBitmapDoneCallback(AsyncOp* pOp)
{
	FTPGetOp* pGetOp = dynamic_cast<FTPGetOp*>(pOp);

	if (pGetOp->Killed())
		return;

	FtpBitmapDownloadComplete(pGetOp);

} //lint !e1746

//----------------------------------------------------------------------------------
// Callback to allow us to update the display every so often.
//----------------------------------------------------------------------------------
void DownloadCtrl::PatchProgressCallback(AsyncOp* pOp)
{
	RecvChunkOp* pChunkOp = dynamic_cast<RecvChunkOp*>(pOp);
	RecvBytesOp* pRcvOp = pChunkOp ? pChunkOp->mRecvBytes : NULL;

	if (pRcvOp)
		UpdateProgressBar(pRcvOp);

} //lint !e1746

//----------------------------------------------------------------------------------
// KillThreads: Blast the transfer threads.
//----------------------------------------------------------------------------------
void DownloadCtrl::KillThreads(void)
{
	if (m_bDownloadStarted && ((! m_bPatchDownloadComplete) || (! m_bBitmapDownloadComplete)))
	{
		// Shut down the download threads.
		if (! m_bPatchDownloadComplete)
		{
			m_bAborted = true;

			SendReport();
			if (m_pHttpGetPatch.get())
				m_pHttpGetPatch->Kill();
			if (m_pFtpGetPatch.get())
				m_pFtpGetPatch->Kill();
		}
		if (! m_bBitmapDownloadComplete)
		{
			if (m_pHttpGetBitmap.get())
				m_pHttpGetBitmap->Kill();
			if (m_pFtpGetBitmap.get())
				m_pFtpGetBitmap->Kill();
		}
	}
}

//----------------------------------------------------------------------------------
// StartDownloads: Start the threads that will download the files.
//----------------------------------------------------------------------------------
void DownloadCtrl::StartDownloads(void)
{
	CustomInfo* pCustInfo = GetCustomInfo();

	CPatchData* pPatch = pCustInfo->GetSelectedPatch();
	if (pPatch)
	{
		#ifdef _DEBUG
			if (GetKeyState(VK_MENU) & GetKeyState(VK_CONTROL) & GetKeyState(VK_SHIFT) & 0x8000)
				pPatch->DebugBox(GetWindow());
		#endif

		GUIString sPatchFile = pPatch->GetPatchUrl();
		int nPos = sPatchFile.rFind('/');
		if (nPos == -1)
			nPos = sPatchFile.rFind('\\');
		if (nPos != -1)
			sPatchFile.erase(0, nPos + 1);
		else
			sPatchFile = TEXT("Update.exe");

		std::string aHostGraphic = pPatch->GetHostBmp();
		int aPos = aHostGraphic.find_last_of('.');
		std::string aHostGraphicExtension;
		if(aPos != std::string::npos && aHostGraphic.length() > aPos + 1)
			aHostGraphicExtension = aHostGraphic.substr(aPos+1);
		else
			aHostGraphicExtension = "bmp";

		m_sHostBitmapFile = GenerateTempFileName("FS", aHostGraphicExtension);

		CreateDirectoryRecursive(pCustInfo->GetPatchFolder());
		GUIString sPatchExe = pCustInfo->GetPatchFolder();
		sPatchExe.append(sPatchFile);

		pCustInfo->SetPatchFile(sPatchExe);

		mStartTime = time(NULL);
		mNumberCallbacks = 0;
		if (IsFtpAddress(pPatch->GetHostBmp()))
		{
			// Don't attempt to handle resumed downloads of bitmaps - not worth it.
			DeleteFile(std::string(m_sHostBitmapFile).c_str());
			m_pFtpGetBitmap = new FTPGetOp(pPatch->GetHostBmp(), true);
			m_pFtpGetBitmap->SetLocalPath(m_sHostBitmapFile);
			m_pFtpGetBitmap->SetCompletion(new DownloadCompletion(FtpBitmapDoneCallback));
			m_pFtpGetBitmap->RunAsync(static_cast<ULONG>(OP_TIMEOUT_INFINITE));
		}
		else
		{
			m_pHttpGetBitmap = new HTTPGetOp(pPatch->GetHostBmp());
			m_pHttpGetBitmap->SetLocalPath(m_sHostBitmapFile);
			m_pHttpGetBitmap->SetCompletion(new DownloadCompletion(HttpBitmapDoneCallback));
			m_pHttpGetBitmap->RunAsync(static_cast<ULONG>(OP_TIMEOUT_INFINITE));
		}

//		m_tDownloadStarted = GetTickCount();

		m_nPrevDownloadedFtpBytes = 0;
		if (IsFtpAddress(pPatch->GetPatchUrl()))
		{
			// If the previous patch profile does not match this patch profile, nuke the file.
			if (! pCustInfo->MatchFtpDownloadTag(sPatchExe, pCustInfo->GetNewVersion(), pPatch->GetPatchSize(), pPatch->GetChecksum()))
				DeleteFile(std::string(sPatchExe).c_str());
			else
				m_nPrevDownloadedFtpBytes = GetFileSize(sPatchExe);

			// Tag the patch's profile (so we can 'resume or not as appropriate later).
			pCustInfo->TagFtpDownload(sPatchExe, pCustInfo->GetNewVersion(), pPatch->GetPatchSize(), pPatch->GetChecksum());

			m_pFtpGetPatch = new FTPGetOp(pPatch->GetPatchUrl(), true);
			m_pFtpGetPatch->SetDoResume(true); // Allow resuming of downloads.
			m_pFtpGetPatch->SetLocalPath(sPatchExe);
			m_pFtpGetPatch->SetRecvChunkCompletion(new DownloadCompletion(PatchProgressCallback), CHUNCK_SIZE);
			m_pFtpGetPatch->SetCompletion(new DownloadCompletion(FtpPatchDoneCallback));
			m_pFtpGetPatch->RunAsync(static_cast<ULONG>(OP_TIMEOUT_INFINITE));
		}
		else
		{
			m_pHttpGetPatch = new HTTPGetOp(pPatch->GetPatchUrl());
			m_pHttpGetPatch->SetLocalPath(sPatchExe);
			m_pHttpGetPatch->SetRecvChunkCompletion(new DownloadCompletion(PatchProgressCallback), CHUNCK_SIZE);
			m_pHttpGetPatch->SetCompletion(new DownloadCompletion(HttpPatchDoneCallback));
			m_pHttpGetPatch->RunAsync(static_cast<ULONG>(OP_TIMEOUT_INFINITE));
		}

		m_bDownloadStarted = true;
	}
}

//----------------------------------------------------------------------------------
// HandleVisitHostButton: Process a click on the Visit Host button.
//----------------------------------------------------------------------------------
bool DownloadCtrl::HandleVisitHostButton(ComponentEvent* pEvent)
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
bool DownloadCtrl::HandleHelpButton(ComponentEvent* pEvent)
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
bool DownloadCtrl::HandleBackButton(ComponentEvent* pEvent)
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
bool DownloadCtrl::HandleNextButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowNextScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void DownloadCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent))           ||
		(pEvent->mComponent == m_pBackButton && HandleBackButton(pEvent))           ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent))           ||
		(pEvent->mComponent == m_pVisitHostButton && HandleVisitHostButton(pEvent)) ||
		(pEvent->mComponent == m_pVisitHostImageButton && HandleVisitHostButton(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}

//----------------------------------------------------------------------------------
// TimerEvent: Handle a timer event.
//----------------------------------------------------------------------------------
bool DownloadCtrl::TimerEvent(int tDelta)
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
