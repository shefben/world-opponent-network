#ifndef __WON_STATUSDIALOG_H__
#define __WON_STATUSDIALOG_H__

#include "WONGUI/Dialog.h"
#include "WONGUI/WONControls.h"

namespace WONAPI
{

WON_PTR_FORWARD(Label);
WON_PTR_FORWARD(Button);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StatusDialog : public Dialog
{
protected:
	LabelPtr mTitleLabel;
	ButtonPtr mCloseButton;
	LabelPtr mStatusLabel;
	ButtonPtr mCancelButton;
	ComponentPtr mDialogComponent;
	WONFrameComponentPtr mBack;

	unsigned long mLastTick;

public:
	StatusDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void SetText(const GUIString &theTitle, const GUIString &theStatus, bool isCancel);
	void SetText(const GUIString &theStatus, bool isCancel);
	void SetTick(unsigned long theTick) { mLastTick = theTick; }
};
typedef SmartPtr<StatusDialog> StatusDialogPtr;

}; // namespace WONAPI

#endif