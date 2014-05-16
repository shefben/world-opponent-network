#ifndef __LANPORTDIALOG_H__
#define __LANPORTDIALOG_H__


#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LanPortDialog : public Dialog
{
protected:
	InputBoxPtr mPortInput;
	LabelPtr mGamePortLabel;
	ButtonPtr mOKButton;
	unsigned short mGamePort;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);
	void UpdateOKButton();
	
	virtual ~LanPortDialog();

public:
	LanPortDialog();

	void SetPort(unsigned short thePort);
	void SetGamePort(unsigned short thePort);

	void Reset();

	unsigned short GetPort(void);
};
typedef SmartPtr<LanPortDialog> LanPortDialogPtr;

}; // namespace WONAPI

#endif