//----------------------------------------------------------------------------------
// AbortDlg.h
//----------------------------------------------------------------------------------
#ifndef __AbortDlg_H__
#define __AbortDlg_H__

#include "WONGUI/MSControls.h"
#include "WONGUI/TextBox.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// AbortDlg.
//----------------------------------------------------------------------------------
class AbortDlg : public Component
{
protected:
	MSMessageBoxPtr m_pMsgBox; // The actual message box.
	WindowPtr       m_pWindow; // The window it uses.

public:
	AbortDlg(Window *pParent, const GUIString& sInfo, const GUIString& sTitle, GUIString sButtonText = GUIString::EMPTY_STR);
	~AbortDlg(void);

	void Close(void);

	virtual void HandleComponentEvent(ComponentEvent* pEvent);
};
typedef SmartPtr<AbortDlg> AbortDlgPtr;

};

#endif
