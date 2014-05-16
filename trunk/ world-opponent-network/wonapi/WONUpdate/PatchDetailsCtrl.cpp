//----------------------------------------------------------------------------------
// PatchDetailsCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "PatchDetailsCtrl.h"
#include "PatchUtils.h"
#include "WONUpdateCtrl.h"
#include "CustomInfo.h"
#include "MessageDlg.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Static member variables.
//----------------------------------------------------------------------------------
PatchDetailsCtrl* PatchDetailsCtrl::m_pInstance = NULL;


//----------------------------------------------------------------------------------
// PatchDetailsCtrl Constructor.
//----------------------------------------------------------------------------------
PatchDetailsCtrl::PatchDetailsCtrl(void)
	: WizardCtrl()
{
	m_pInstance = this;
	m_bDownloadSucceeded = false;
}

//----------------------------------------------------------------------------------
// PatchDetailsCtrl Destructor.
//----------------------------------------------------------------------------------
PatchDetailsCtrl::~PatchDetailsCtrl(void)
{
	m_pInstance = NULL;

	if (m_sInfoFile != "")
		DeleteFile(m_sInfoFile.c_str());
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::InitComponent(ComponentInit &theInit)
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

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetWelcomeHelpCallback() != NULL);
	}
}

//----------------------------------------------------------------------------------
// EnableControls: Enable or disable controls as appropriate.
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::EnableControls(void)
{
//	m_pNextButton->Enable(m_bDownloadSucceeded);
	m_pNextButton->Enable(true);

	if (m_bDownloadSucceeded)
		m_pNextButton->RequestFocus();
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::Show(bool bShow)
{
	SetVisible(bShow);

	if (bShow)
	{
		CustomInfo* pCI = GetCustomInfo();

		// Check to see if we have already downloaded these patch details.
		if (m_bDownloadSucceeded && (m_sURL == pCI->GetPatchDescriptionUrl()))
		{
			// We already have the file, re-display it.
			LoadInfoTextFromFile(m_sInfoFile);

			// Make sure the Next button is the default button.
			m_pNextButton->RequestFocus();
			return;
		}

		// Some of the text changes dynamically based on patch selection, update it.
		ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

		// Informational (Main) text.
		GUIString sInfo = pResMgr->BuildInfoString(PatchDetails_Info1_String_Id);
		m_pInfoText->Clear();
		m_pInfoText->AddFormatedText(sInfo);
		m_pInfoText->SetVertOffset(0); // Scroll to the top.
		m_pInfoText->Invalidate();

		// Reset the download variables.
		m_bDownloadStarted = false;
		m_bDownloadComplete = false;
		m_bDownloadSucceeded = false;
		m_bAborted = false;
		m_sURL = pCI->GetPatchDescriptionUrl();

		EnableControls();

		// Start the download itself.
		m_pBackButton->RequestFocus();
		StartDownload();
	}
	else
	{
		// If the download is still in progress, kill it.
		KillThreads();
	}
}

//----------------------------------------------------------------------------------
// LoadInfoTextFromFile: Load the info box from the downloaded file.
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::LoadInfoTextFromFile(const std::string& sFile)
{
	FILE* pFile = fopen(sFile.c_str(), "rt+");
	if (pFile)
	{
		ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

		const int MAX_INFO_FILE_SIZE = 100000;
		char sRawContents[MAX_INFO_FILE_SIZE] = "";
		memset(sRawContents, 0, MAX_INFO_FILE_SIZE);
		UINT nRead = static_cast<UINT>(fread(sRawContents, 1, sizeof(sRawContents) - 1, pFile));
		if (nRead == sizeof(sRawContents) - 1)
		{
			char sRead[32];
			wsprintf(sRead, "%d", nRead);
			GUIString sFormat = pResMgr->GetFinishedString(PatchDetails_FileTooBig_String);
			MessageBox(GetWindow(), sFormat, pResMgr->GetFinishedString(PatchDetails_FileTooBigTtl_String), MD_OK);
		}
		else
			sRawContents[nRead] = 0;

		if (nRead > 0)
		{
			m_pInfoText->Clear();
			m_pInfoText->AddFormatedText(sRawContents);
			m_pInfoText->SetVertOffset(0); // Scroll to the top.
			m_pInfoText->Invalidate();
		}

		fclose(pFile);
	}
}

//----------------------------------------------------------------------------------
// HttpDownloadComplete: The http transfer has completed for the patch.  Check the 
// status to see if it worked.
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::HttpDownloadComplete(HTTPGetOp* pGetOp)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	WONStatus aStatus = pGetOp->GetStatus();

	if (aStatus == WS_Success)
	{
		if (! m_bAborted)
			LoadInfoTextFromFile(m_sInfoFile);

		m_bDownloadSucceeded = true;
	}
	else
	{
		if (! m_bAborted) // If the user aborted don't pop up an error telling them they aborted...
		{
			// Clear the file name (so no one tries to use it), but don't deleted it, 
			// we can resume the download later.
///			pCustInfo->SetPatchFile("");

			GUIString sTitle = pResMgr->GetFinishedString(Download_DownloadError_String);
			GUIString sMsg;

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
				default:
					sMsg = pResMgr->GetFinishedString(Download_UnknownHttpError_String);
					ReplaceSubString(sMsg, "%ERROR%", WONStatusToString(aStatus));
					MessageBox(GetWindow(), sMsg, sTitle, MD_OK);
			} //lint !e788
		}
	}

	m_bDownloadComplete = true;
	EnableControls();
}

//----------------------------------------------------------------------------------
// FtpDownloadComplete: The ftp transfer has completed for the patch.  Check the 
// status to see if it worked.
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::FtpDownloadComplete(FTPGetOp* pGetOp)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	WONStatus aStatus = pGetOp->GetStatus();

	if (aStatus == WS_Success)
	{
		if (! m_bAborted)
			LoadInfoTextFromFile(m_sInfoFile);

		m_bDownloadSucceeded = true;
	}
	else
	{
		if (! m_bAborted) // If the user aborted don't pop up an error telling them they aborted...
		{
			// Clear the file name (so no one tries to use it), but don't deleted it, 
			// we can resume the download later.
///			pCustInfo->SetPatchFile("");

			GUIString sTitle = pResMgr->GetFinishedString(Download_DownloadError_String);
			GUIString sMsg;

			switch (aStatus)
			{
				case WS_FTP_StatusError:
					switch (pGetOp->GetFTPStatus())
					{
						case 500:
							MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_InvalidFtpUser_String), sTitle, MD_OK);
							break;
						case 530:
							MessageBox(GetWindow(), pResMgr->GetFinishedString(Download_InvalidFtpPassword_String), sTitle, MD_OK);
							break;
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
		}
	}

	m_bDownloadComplete = true;
	EnableControls();
}

//----------------------------------------------------------------------------------
// HttpDownloadDoneCallback: Download has completed (sucessfuly or otherwise).
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::HttpDownloadDoneCallback(AsyncOp* pOp)
{
	HTTPGetOp* pGetOp = dynamic_cast<HTTPGetOp*>(pOp);

	if (pGetOp->Killed())
		return;

	HttpDownloadComplete(pGetOp);

} //lint !e1746

//----------------------------------------------------------------------------------
// FtpDownloadDoneCallback: Download has completed (sucessfuly or otherwise).
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::FtpDownloadDoneCallback(AsyncOp* pOp)
{
	FTPGetOp* pGetOp = dynamic_cast<FTPGetOp*>(pOp);

	if (pGetOp->Killed())
		return;

	FtpDownloadComplete(pGetOp);

} //lint !e1746

//----------------------------------------------------------------------------------
// KillThreads: Blast the transfer threads.
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::KillThreads(void)
{
	if (m_bDownloadStarted && ! m_bDownloadComplete)
	{
		m_bAborted = true;

		// Shut down the download threads.
		if (! m_bDownloadComplete)
		{
			if (m_pHttpGetFile.get())
				m_pHttpGetFile->Kill();
			if (m_pFtpGetFile.get())
				m_pFtpGetFile->Kill();
		}
	}
}

//----------------------------------------------------------------------------------
// StartDownload: Start the thread that will download the file.
//----------------------------------------------------------------------------------
void PatchDetailsCtrl::StartDownload(void)
{
	CustomInfo* pCustInfo = GetCustomInfo();

	if (m_sURL != "")
	{
		CreateDirectoryRecursive(pCustInfo->GetPatchFolder());
		m_sInfoFile = pCustInfo->GetPatchFolder();
		m_sInfoFile.append("PatchDetails.txt");

		// Remove it (make sure we don't accidentaly use some old leftover).
		DeleteFile(m_sInfoFile.c_str());

		if (IsFtpAddress(m_sURL))
		{
			m_pFtpGetFile = new FTPGetOp(m_sURL, true);
			m_pFtpGetFile->SetLocalPath(m_sInfoFile);
			m_pFtpGetFile->SetCompletion(new PatchDetailsCompletion(FtpDownloadDoneCallback));
			m_pFtpGetFile->RunAsync(static_cast<ULONG>(OP_TIMEOUT_INFINITE));
		}
		else
		{
			m_pHttpGetFile = new HTTPGetOp(m_sURL);
			m_pHttpGetFile->SetLocalPath(m_sInfoFile);
			m_pHttpGetFile->SetCompletion(new PatchDetailsCompletion(HttpDownloadDoneCallback));
			m_pHttpGetFile->RunAsync(static_cast<ULONG>(OP_TIMEOUT_INFINITE));
		}

		m_bDownloadStarted = true;
	}
}

//----------------------------------------------------------------------------------
// HandleHelpButton: Process a click on the Help button.
//----------------------------------------------------------------------------------
bool PatchDetailsCtrl::HandleHelpButton(ComponentEvent* pEvent)
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
bool PatchDetailsCtrl::HandleBackButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	// If the Patch Details have not been downloaded, the thread will be closed when 
	// Show is called to hide this window.

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowPreviousScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleNextButton: Process a click on the Next button.
//----------------------------------------------------------------------------------
bool PatchDetailsCtrl::HandleNextButton(ComponentEvent* pEvent)
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
void PatchDetailsCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent)) ||
		(pEvent->mComponent == m_pBackButton && HandleBackButton(pEvent)) ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent)) ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}
