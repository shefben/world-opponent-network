#ifndef __CREDITSDIALOG_H__
#define __CREDITSDIALOG_H__

#include "WONGUI/Dialog.h"
#include "WONGUI/WONGUIConfig/DialogConfig.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(ScrollingTextComponent);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CreditsDialog : public Dialog
{
protected:
	ScrollingTextComponentPtr mScrollingText;
	ButtonPtr                 mCreditsButton;
	ButtonPtr                 mCloseButton;
	
	void UpdateCredits();

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	CreditsDialog();

	virtual void InitComponent(ComponentInit &theInit);

	void Start(); 
	void Stop();  
	void Pause(); 
	void Reset(); 
	void AddLine(const GUIString &theText, Component* theComponent, DWORD theColor = -1);

};
typedef SmartPtr<CreditsDialog> CreditsDialogPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CreditsDialogConfig : public DialogConfig
{
protected:
	CreditsDialog* mCredits;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { DialogConfig::SetComponent(theComponent); mCredits = (CreditsDialog*)theComponent; }

	static ComponentConfig* CfgFactory() { return new CreditsDialogConfig; }
};


}; // namespace WONAPI

#endif