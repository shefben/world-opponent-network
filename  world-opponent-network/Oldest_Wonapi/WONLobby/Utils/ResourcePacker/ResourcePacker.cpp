// ResourcePacker.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ResourcePacker.h"
#include "ResourcePackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourcePackerApp

BEGIN_MESSAGE_MAP(CResourcePackerApp, CWinApp)
	//{{AFX_MSG_MAP(CResourcePackerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourcePackerApp construction

CResourcePackerApp::CResourcePackerApp() {}

/////////////////////////////////////////////////////////////////////////////
// The one and only CResourcePackerApp object

CResourcePackerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CResourcePackerApp initialization

BOOL CResourcePackerApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CResourcePackerDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
