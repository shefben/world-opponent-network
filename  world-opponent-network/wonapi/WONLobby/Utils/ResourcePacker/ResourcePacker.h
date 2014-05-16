// ResourcePacker.h : main header file for the RESOURCEPACKER application
//

#if !defined(AFX_RESOURCEPACKER_H__070CB0DD_F163_44CA_AA7B_964C7FCD8A56__INCLUDED_)
#define AFX_RESOURCEPACKER_H__070CB0DD_F163_44CA_AA7B_964C7FCD8A56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CResourcePackerApp:
// See ResourcePacker.cpp for the implementation of this class
//

class CResourcePackerApp : public CWinApp
{
public:
	CResourcePackerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResourcePackerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CResourcePackerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESOURCEPACKER_H__070CB0DD_F163_44CA_AA7B_964C7FCD8A56__INCLUDED_)
