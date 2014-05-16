#ifndef __WON_QUICKPLAYCTRL_H__
#define __WON_QUICKPLAYCTRL_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class QuickPlayCtrl : public Dialog
{
protected:
	LabelPtr			mWelcomeLabel;
	ButtonPtr			mNoviceButton;
	ButtonPtr			mIntermediateButton;
	ButtonPtr			mAdvancedButton;
	ButtonPtr			mExpertButton;
	ButtonPtr			mPlayButton;
	
public:
	void HandleEnteredScreen();

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);

public:
	QuickPlayCtrl();	
};
typedef SmartPtr<QuickPlayCtrl> QuickPlayCtrlPtr;

}; // namespace WONAPI


#endif