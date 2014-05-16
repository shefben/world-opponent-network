//----------------------------------------------------------------------------------
// AbortDlg.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/SimpleComponent.h"
#include "AbortDlg.h"
#include "WizardCtrl.h"
#include "CustomInfo.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// AbortDlg Constructor
//----------------------------------------------------------------------------------
AbortDlg::AbortDlg(Window *pParent, const GUIString& sInfo, const GUIString& sTitle, GUIString sButtonText)
{
}

//----------------------------------------------------------------------------------
// AbortDlg Destructor
//----------------------------------------------------------------------------------
AbortDlg::~AbortDlg(void)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	if (sButtonText == GUIString::EMPTY_STR)
		sButtonText = pResMgr->GetString(IDS_ABORT);

	// Constuct the Message Box.
//	m_pMsgBox = new MSMessageBox(sTitle, sMsg, sButtonText, GUIString::EMPTY_STR, GUIString::EMPTY_STR);
	m_pMsgBox = new MSMessageBox(sTitle, sMsg, sButtonText);

	// Set the background image.
	NativeImagePtr pBackroundImg = pResMgr->GetBackgroundImage(IDB_MESSAGE_DLG_BACKGROUND);
	ImageComponentPtr pBackground = new ImageComponent(pBackroundImg, true);
	m_pMsgBox->GetLabel()->SetTransparent(true);
	m_pMsgBox->AddChildLayout(pBackground, CLI_SameSize, m_pMsgBox);
	m_pMsgBox->AddChildEx(pBackground, 0);

	// Prepare the Message Box.
	m_pWindow = m_pMsgBox->PrepareWindow(pParent);
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
void AbortDlg::~AbortDlg(void)
{
	Close();
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
void AbortDlg::Close(void)
{
	m_pWindow->Close(); // An extra call won't hurt.
}

