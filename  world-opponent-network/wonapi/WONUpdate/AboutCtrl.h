//----------------------------------------------------------------------------------
// AboutCtrl.h
//----------------------------------------------------------------------------------
#ifndef __AboutCtrl_H__
#define __AboutCtrl_H__

#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// AboutCtrl.
//----------------------------------------------------------------------------------
class AboutCtrl : public MSDialog
{
protected:
	GUIString   m_sVersion;        // Version string (inserted into text).
	MSLabelPtr  m_pVersionLabel;   // Version control (text control).
	MSLabelPtr  m_pCopyrightLabel; // Copyright control (text control).
	ButtonPtr   m_pCloseButton;    // Close Button.
	bool        m_IsVersionSet;

public:
	AboutCtrl(void);
	~AboutCtrl(void);

	void SetVersion(const GUIString& sVer) { m_sVersion = sVer; }

	int DoDialog(Window *pParent = NULL);

	bool HandleCloseButton(ComponentEvent* pEvent);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<AboutCtrl> AboutCtrlPtr;

};

#endif
