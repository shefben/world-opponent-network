//----------------------------------------------------------------------------------
// AboutDlg.h
//----------------------------------------------------------------------------------
#ifndef __AboutDlg_H__
#define __AboutDlg_H__

#include "WONGUI/MSControls.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// AboutDlg.
//----------------------------------------------------------------------------------
class AboutDlg : public MSDialog
{
protected:
	GUIString   m_sVersion;        // Version string (inserted into text).
	MSLabelPtr  m_pVersionLabel;   // Version control (text control).
	MSLabelPtr  m_pCopyrightLabel; // Copyright control (text control).
	ButtonPtr   m_pCloseButton;    // Close Button.

public:
	AboutDlg(void);
	~AboutDlg(void);

	void SetVersion(const GUIString& sVer) { m_sVersion = sVer; }

	int DoDialog(Window *pParent = NULL);

	bool HandleCloseButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);
};
typedef SmartPtr<AboutDlg> AboutDlgPtr;

};

#endif
