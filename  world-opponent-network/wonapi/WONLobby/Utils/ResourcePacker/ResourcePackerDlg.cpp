// ResourcePackerDlg.cpp : implementation file
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
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourcePackerDlg dialog

CResourcePackerDlg::CResourcePackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResourcePackerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResourcePackerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CResourcePackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResourcePackerDlg)
	DDX_Control(pDX, IDC_RESOURCE_LIST, m_ResourceListCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CResourcePackerDlg, CDialog)
	//{{AFX_MSG_MAP(CResourcePackerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADD_FILE, OnAddFile)
	ON_BN_CLICKED(IDC_ADD_NEW_FILE, OnAddNewFile)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_REMOVE_FILE, OnRemoveFile)
	ON_BN_CLICKED(IDC_WRITE, OnWrite)
	ON_BN_CLICKED(IDC_UPDATE_FILE, OnUpdateFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourcePackerDlg message handlers

BOOL CResourcePackerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CImageList  *pImageList = new CImageList();
	CResourcePackerApp *pApp = (CResourcePackerApp *)AfxGetApp();
	pImageList->Create(12, 12, ILC_MASK, 4, 4);
	pImageList->Add(pApp->LoadIcon(IDI_BLANK));
	pImageList->Add(pApp->LoadIcon(IDI_BLUE_CHECK));
	pImageList->Add(pApp->LoadIcon(IDI_RED_X));
	pImageList->Add(pApp->LoadIcon(IDI_GREEN_PLUS));
	m_ResourceListCtrl.SetImageList(pImageList, LVSIL_SMALL);
	
	ListView_SetExtendedListViewStyleEx(m_ResourceListCtrl.m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	int aCol(0);
	m_ResourceListCtrl.InsertColumn(aCol, "Name", LVCFMT_LEFT, m_ResourceListCtrl.GetStringWidth("Name") + 20, aCol++);
	m_ResourceListCtrl.InsertColumn(aCol, "Size", LVCFMT_RIGHT, m_ResourceListCtrl.GetStringWidth("Size") + 20, aCol++);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CResourcePackerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CResourcePackerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CResourcePackerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CResourcePackerDlg::OnAddFile() 
{
	int anIndex;
	CString aFileName;
	POSITION aPos = m_ResourceListCtrl.GetFirstSelectedItemPosition();
	while(aPos)
	{
		anIndex = m_ResourceListCtrl.GetNextSelectedItem(aPos);
		aFileName = m_ResourceListCtrl.GetItemText(anIndex, 0);
		mResourceCollection.AddResourceItem(aFileName.operator LPCTSTR(), false);
	}
	UpdateList();
}

void CResourcePackerDlg::OnAddNewFile() 
{
	CFileDialog aFileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "Resource File(s) (*.*)|*.*||");
	aFileDlg.m_ofn.lpstrTitle = "Select Resource File(s) To Add to Collection";
	aFileNameBufferP[0] = '\0';
	aFileDlg.m_ofn.lpstrFile = aFileNameBufferP;
	aFileDlg.m_ofn.nMaxFile = 64000;
    if(aFileDlg.DoModal() == IDOK)
	{
		POSITION aPos = aFileDlg.GetStartPosition();
		while(aPos)
			mResourceCollection.AddResourceItem(aFileDlg.GetNextPathName(aPos).operator LPCTSTR(), false);

		UpdateList();
	}
}

void CResourcePackerDlg::OnOpen() 
{
	CFileDialog aFileDlg(TRUE, "lob", NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "WON Resource Collection Files (*.LOB)|*.lob||");
	aFileDlg.m_ofn.lpstrTitle = "Select WON Resource Collection File";
    if(aFileDlg.DoModal() == IDOK)
	{
		mResourceCollection.OpenCollection(aFileDlg.GetPathName().operator LPCTSTR());
		UpdateList();
	}
}

void CResourcePackerDlg::OnRemoveFile() 
{
	int anIndex;
	CString aFileName;
	POSITION aPos = m_ResourceListCtrl.GetFirstSelectedItemPosition();
	while(aPos)
	{
		anIndex = m_ResourceListCtrl.GetNextSelectedItem(aPos);
		aFileName = m_ResourceListCtrl.GetItemText(anIndex, 0);
		mResourceCollection.RemoveResourceItem(aFileName.operator LPCTSTR());
	}
	UpdateList();
}

void CResourcePackerDlg::OnWrite() 
{
	CFileDialog aFileDlg(FALSE, "lob", NULL, OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, "WON Resource Collection Files (*.LOB)|*.lob||");
	aFileDlg.m_ofn.lpstrTitle = "Save WON Resource Collection File";
    if(aFileDlg.DoModal() == IDOK)
	{
		mResourceCollection.WriteCollection(aFileDlg.GetPathName().operator LPCTSTR());
		UpdateList();
	}
}

void CResourcePackerDlg::UpdateList(void)
{
	m_ResourceListCtrl.DeleteAllItems();

	int anIcon = 0;
	int anIndex = 0;
	int aWidth;
	char aBuffer[50];
	std::string anItemName = mResourceCollection.GetFirstResourceItem();
	while(!anItemName.empty())
	{
		const WONAPI::WONResourceCollection::WON_RESOURCE_ITEM* anItemP = mResourceCollection.GetResourceItem(anItemName);
		switch(anItemP->mStatus)
		{
		case WONAPI::WONResourceCollection::STATUS_TO_ADD:
			anIcon = 3;
			break;
		case WONAPI::WONResourceCollection::STATUS_TO_REMOVE_ADD:
		case WONAPI::WONResourceCollection::STATUS_TO_REMOVE_OK:
			anIcon = 2;
			break;
		case WONAPI::WONResourceCollection::STATUS_OK:
			anIcon = 1;
			break;
		case WONAPI::WONResourceCollection::STATUS_BLANK:
		default:
			anIcon = 0;
			break;
		}

		anIndex = m_ResourceListCtrl.InsertItem(anIndex++, anItemName.c_str(), anIcon);
		if(anIndex != -1)
		{
			aWidth = m_ResourceListCtrl.GetStringWidth(anItemName.c_str()) + 20;
			if(aWidth > m_ResourceListCtrl.GetColumnWidth(0))
				m_ResourceListCtrl.SetColumnWidth(0, aWidth);

			sprintf(aBuffer, "%d", anItemP->mDataP->length());
			m_ResourceListCtrl.SetItem(anIndex, 1, LVIF_TEXT, aBuffer, NULL, NULL, NULL, NULL);
			aWidth = m_ResourceListCtrl.GetStringWidth(aBuffer) + 20;
			if(aWidth > m_ResourceListCtrl.GetColumnWidth(1))
				m_ResourceListCtrl.SetColumnWidth(1, aWidth);
		}
		anItemName = mResourceCollection.GetNextResourceItem();
	}
}

void CResourcePackerDlg::OnUpdateFile() 
{
	CFileDialog aFileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "Resource File(s) (*.*)|*.*||");
	aFileDlg.m_ofn.lpstrTitle = "Select Resource File(s) To Update in Collection";
	aFileNameBufferP[0] = '\0';
	aFileDlg.m_ofn.lpstrFile = aFileNameBufferP;
	aFileDlg.m_ofn.nMaxFile = 64000;
    if(aFileDlg.DoModal() == IDOK)
	{
		POSITION aPos = aFileDlg.GetStartPosition();
		while(aPos)
			mResourceCollection.AddResourceItem(aFileDlg.GetNextPathName(aPos).operator LPCTSTR(), true);

		UpdateList();
	}
}
