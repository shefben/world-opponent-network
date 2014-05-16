//----------------------------------------------------------------------------------
// LoginCtrl.h
//----------------------------------------------------------------------------------
#ifndef __LoginCtrl_H__
#define __LoginCtrl_H__

#include "WONGUI/TextBox.h"
#include "WONGUI/MSControls.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// LoginCtrl.
//----------------------------------------------------------------------------------
class LoginCtrl : public MSDialog
{
protected:
	InputBoxPtr m_pLoginNameInputBox;// Login name.
	InputBoxPtr m_pPasswordInputBox; // Password.
	ButtonPtr   m_pCancelButton;     // Cancel Button.
	ButtonPtr   m_pLoginButton;      // Login Button.
	MSLabelPtr  m_pText;			 // Descriptive text

public:
	LoginCtrl(void);
	~LoginCtrl(void);

	GUIString GetUserName(void) { return m_pLoginNameInputBox->GetText(); }
	GUIString GetPassword(void) { return m_pPasswordInputBox->GetText(); }

	int DoDialog(const GUIString& theTitle, const GUIString& theText, Window *pParent = NULL);

	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<LoginCtrl> LoginCtrlPtr;

};

#endif
