//----------------------------------------------------------------------------------
// SelectHostCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "SelectHostCtrl.h"
#include "WONUpdateCtrl.h"
#include "CustomInfo.h"
#include "DebugLog.h"
#include "MessageDlg.h"
#include "PatchUtils.h"
#include "PatchCore.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// PatchDataItem.
//----------------------------------------------------------------------------------
class PatchDataItem : public MultiListItem
{
public:
	CPatchData* m_pPatchData;

	PatchDataItem(void) { m_pPatchData = NULL; }
};
typedef SmartPtr<PatchDataItem> PatchDataItemPtr;


//----------------------------------------------------------------------------------
// Static member variables.
//----------------------------------------------------------------------------------
SelectHostCtrl* SelectHostCtrl::m_pInstance = NULL;


//----------------------------------------------------------------------------------
// SelectHostCtrl Constructor.
//----------------------------------------------------------------------------------
SelectHostCtrl::SelectHostCtrl(void)
	: WizardCtrl()
{
	m_pInstance = this;

	PatchCore* pCore = GetMainControl()->GetPatchCore();
	pCore->SetSelfVersionCheckCallback(new SelectHostCtrlCallback(SelfVersionCheckCallback));
	pCore->SetGameVersionCheckCallback(new SelectHostCtrlCallback(GameVersionCheckCallback));
}

//----------------------------------------------------------------------------------
// SelectHostCtrl Destructor.
//----------------------------------------------------------------------------------
SelectHostCtrl::~SelectHostCtrl(void)
{
	m_pInstance = NULL;
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void SelectHostCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
	
		WONComponentConfig_Get(aConfig,m_pInfoText,"InformationText");
		WONComponentConfig_Get(aConfig,m_pHostList,"HostListArea");
		WONComponentConfig_Get(aConfig,m_pConfigProxyButton,"ConfigureProxyButton");
		WONComponentConfig_Get(aConfig,m_pAlreadyHaveUpdateButton,"AlreadyHaveUpdateButton");
		WONComponentConfig_Get(aConfig,m_pHelpButton,"HelpButton");
		WONComponentConfig_Get(aConfig,m_pBackButton,"BackButton");
		WONComponentConfig_Get(aConfig,m_pNextButton,"NextButton");

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetWelcomeHelpCallback() != NULL);
	}
}

//----------------------------------------------------------------------------------
// UpdatePatchList: Update the list of patches (and results).
//----------------------------------------------------------------------------------
void SelectHostCtrl::UpdatePatchList(void)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	GUIString sError;
	int nManualPatches = 0;
	int nAutoPatches = 0;

	// Clear the list box of old refuse.
	m_pHostList->Clear();

	m_pHostList->BeginMultiChange();

	GUIString sDefault = pCustInfo->LoadDefaultSelection();
	sDefault.toLowerCase();
	CPatchDataList* pPatches = pCustInfo->GetPatchList();

	// Go through the list and decide if we have to mark manual downloads.
	CPatchDataList::iterator Itr = pPatches->begin();

	while (Itr != pPatches->end())
	{
		CPatchData* pData = *Itr;

		if (pData)
		{
			if (pData->GetMustVisitHost())
				nManualPatches++;
			else
				nAutoPatches++;
		}

		++Itr;
	}

	m_bMixedPatches = nManualPatches != 0 && nAutoPatches != 0;

	// Now go through the list and add all of the items to the list box.
	Itr = pPatches->begin();
	int nIndex = 0;
	int nSelectIndex = -1;

	while (Itr != pPatches->end())
	{
		CPatchData* pData = *Itr;
		//pData->MessageBox(GetWindow()); // Debugging Aide.

		if (pData)
		{
			// Start with the Patch Name (we will display this).
			std::string sLine = pData->GetPatchName();

			// Add the Manual flag (if needed).
			if (pData->GetMustVisitHost() && m_bMixedPatches)
				sLine += " *";

			// Now add the results of previous download atempts.
			int nFailures = pData->GetDownloadFailures();
			int nAborts = pData->GetDownloadAborts();

			if (nFailures)
			{
				sError = pResMgr->GetFinishedString(Global_DownloadFailures_String);
				ReplaceSubInt(sError, "%NUM_FAILURES%", nFailures);
			}
			else if (nAborts)
			{
				sError = pResMgr->GetFinishedString(Global_DownloadAborts_String);
				ReplaceSubInt(sError, "%NUM_ABORTS%", nAborts);
			}
			else
				sError = "";

			PatchDataItem* pItem = static_cast<PatchDataItem*>(m_pHostList->InsertItem(new PatchDataItem));
			m_pHostList->SetString(nIndex, 0, sLine);
			m_pHostList->SetString(nIndex, 1, sError);
			pItem->m_pPatchData = pData;

			// Check to see if this was the last 'successful download site', if so, select it.
			GUIString sHost = pData->GetHostName();
			sHost.toLowerCase();
			if (sDefault == sHost)
				nSelectIndex = nIndex;

			++nIndex;
		}

		++Itr;
	}

	// If no site was selected, choose one at random.
	if (nSelectIndex == -1 && nIndex > 0)
	{
		srand(GetTickCount());
		nSelectIndex = rand() % nIndex;
	}

	// Actually make the selection.
	if (nSelectIndex != -1)
		m_pHostList->SetSelItem(nSelectIndex);

	m_pHostList->EndMultiChange();

	// Changing the patch list can change the Info Text, so update it.
	UpdateInfoText();
	EnableControls();
}

//----------------------------------------------------------------------------------
// EnableControls: Enable or disable the controls as aproproiate.
//----------------------------------------------------------------------------------
void SelectHostCtrl::EnableControls(void)
{
	m_pNextButton->Enable(m_pHostList->GetSelItem() != NULL);
}

//----------------------------------------------------------------------------------
// SelfVersionCheckCallback: The self (SierraUp) version check operation has 
// completed.
//----------------------------------------------------------------------------------
void SelectHostCtrl::SelfVersionCheckCallback(OperationInfo* pOpInfo)
{
	UpdatePatchList();
}

//----------------------------------------------------------------------------------
// GameVersionCheckCallback: The game's version check operation has completed.
//----------------------------------------------------------------------------------
void SelectHostCtrl::GameVersionCheckCallback(OperationInfo* pOpInfo)
{
	UpdatePatchList();
}

//----------------------------------------------------------------------------------
// UpdateInfoText: Update the contents of the info text box.
//----------------------------------------------------------------------------------
void SelectHostCtrl::UpdateInfoText(void)
{
	// Some of the text changes dynamically based on patch selection, update it.
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	GUIString sInfo = pResMgr->BuildInfoString(SelectHost_WaitInfo1_String_Id);

	if (GetCustomInfo()->GetPatchList()->size())
	{
		sInfo = pResMgr->BuildInfoString(SelectHost_Info1_String_Id);

		if (m_bMixedPatches)
		{
			sInfo.append("\n\n");
			sInfo.append(pResMgr->GetFinishedString(SelectHost_InfoManual_String));
		}
	}

	m_pInfoText->Clear();
	m_pInfoText->AddFormatedText(sInfo);
	m_pInfoText->SetVertOffset(0); // Scroll to the top.
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void SelectHostCtrl::Show(bool bShow)
{
	SetVisible(bShow);

	if (bShow)
	{
		m_sLocalPatch = "";
		GetCustomInfo()->SetPatchFile("");

		UpdateInfoText();
		UpdatePatchList();
		EnableControls();

		// Make sure the appropriate control has the focus.
		if (m_pHostList->GetSelItem())
			m_pNextButton->RequestFocus();
		else
			m_pHostList->RequestFocus();
	}
}

//----------------------------------------------------------------------------------
// HandleAlreadyHaveUpdateButton: Process a click on the Already Have Update button.
//----------------------------------------------------------------------------------
bool SelectHostCtrl::HandleAlreadyHaveUpdateButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	MSFileDialog Dlg;
	Dlg.SetFileMustExist(true);
	Dlg.AddFilter("*.exe", pResMgr->GetFinishedString(SelectHost_PatchFilter_String));
	Dlg.SetTitle(pResMgr->GetFinishedString(SelectHost_FileOpenDlgTitle_String));
	//Dlg.SetOkTitle("Open"); // No reason to change the default.
	Dlg.SetFileMustExist(true);
	Dlg.SetNoChangeDir(true);
	Dlg.SetInitialDir(GetCustomInfo()->GetPatchFolder());

	if (Dlg.DoDialog(GetWindow()))
	{
		m_sLocalPatch = Dlg.GetFile();
		if (GetMainControl()->GetPatchCore()->MatchLocalPatch(GetWindow(), m_sLocalPatch))
			FireNextButton();
		else
			m_sLocalPatch = "";
	}

	return true;
}

//----------------------------------------------------------------------------------
// HandleConfigureProxyButton: Process a click on the Configure Proxy button.
//----------------------------------------------------------------------------------
bool SelectHostCtrl::HandleConfigProxyButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
		pUpdateControl->ShowScreen(WONUpdateCtrl::WS_ConfigProxy);

	return true;
}

//----------------------------------------------------------------------------------
// HandleHelpButton: Process a click on the Help button.
//----------------------------------------------------------------------------------
bool SelectHostCtrl::HandleHelpButton(ComponentEvent* pEvent)
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
bool SelectHostCtrl::HandleBackButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	// Send the user to the previous screen.
	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowPreviousScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleNextButton: Process a click on the Next button.
//----------------------------------------------------------------------------------
bool SelectHostCtrl::HandleNextButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	// Extract the selected patch.
	PatchDataItem* pItem = static_cast<PatchDataItem*>(m_pHostList->GetSelItem());

	// Tag this patch so we know what we're dealing with later.
	GetCustomInfo()->SetSelectedPatch(pItem ? pItem->m_pPatchData : NULL);

	// Send the user to the next screen (it will vary based on the selected host).
	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowNextScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleHostList: Process a change in the Host List.
//----------------------------------------------------------------------------------
bool SelectHostCtrl::HandleHostList(ComponentEvent* pRawEvent)
{
	// Check for a simple click or selection change.
	if (pRawEvent->mType == ComponentEvent_ListSelChanged)
	{
		// Extract the selected patch.
		PatchDataItem* pItem = static_cast<PatchDataItem*>(m_pHostList->GetSelItem());

		// Tag the selected patch so we can fill in the appropriate text.
		GetCustomInfo()->SetSelectedPatch(pItem ? pItem->m_pPatchData : NULL);

		UpdateInfoText();

		EnableControls();

		// Make sure we validate the patch (this can be turned off if the user uses the Already Have Update feature).
		GetMainControl()->GetPatchCore()->SetSkipValidation(false);
		return true;
	}

	// Look for a double click.
	ListItemClickedEvent* pEvent = dynamic_cast<ListItemClickedEvent*>(pRawEvent);
	if (pEvent && pEvent->mType == ComponentEvent_ListItemClicked && pEvent->mWasDoubleClick)
	{
		FireNextButton();
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void SelectHostCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent)) ||
		(pEvent->mComponent == m_pBackButton && HandleBackButton(pEvent)) ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent)) ||
		(pEvent->mComponent == m_pConfigProxyButton && HandleConfigProxyButton(pEvent)) ||
		(pEvent->mComponent == m_pAlreadyHaveUpdateButton && HandleAlreadyHaveUpdateButton(pEvent)) ||
		(pEvent->mComponent == m_pHostList && HandleHostList(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}
