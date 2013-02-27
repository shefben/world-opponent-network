// ResourcePackerDlg.h : header file
//

#if !defined(AFX_RESOURCEPACKERDLG_H__E7128197_4CE1_4EE8_804C_76E42D1160DE__INCLUDED_)
#define AFX_RESOURCEPACKERDLG_H__E7128197_4CE1_4EE8_804C_76E42D1160DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CResourcePackerDlg dialog
#include "WONGUI/ResourceCollection.h"

class CResourcePackerDlg : public CDialog
{
// Construction
public:
	CResourcePackerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CResourcePackerDlg)
	enum { IDD = IDD_RESOURCEPACKER_DIALOG };
	CListCtrl	m_ResourceListCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResourcePackerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	WONAPI::WONResourceCollection mResourceCollection;
	char aFileNameBufferP[64000];

	void UpdateList(void);

	// Generated message map functions
	//{{AFX_MSG(CResourcePackerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAddFile();
	afx_msg void OnAddNewFile();
	afx_msg void OnOpen();
	afx_msg void OnRemoveFile();
	afx_msg void OnWrite();
	afx_msg void OnUpdateFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESOURCEPACKERDLG_H__E7128197_4CE1_4EE8_804C_76E42D1160DE__INCLUDED_)
