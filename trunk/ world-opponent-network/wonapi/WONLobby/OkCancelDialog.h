#ifndef __WON_OKCANCELDIALOG_H__
#define __WON_OKCANCELDIALOG_H__

#include "WONGUI/Dialog.h"
#include "WONGUI/WONControls.h"

namespace WONAPI
{

WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class OkCancelDialog : public Dialog
{
protected:
	LabelPtr mTitleLabel;
	LabelPtr mBodyLabel;
	ButtonPtr mOkButton;
	ButtonPtr mCancelButton;
	ButtonPtr mCloseButton;
	ComponentPtr mDialogComponent;
	WONFrameComponentPtr mBack;

public:
	OkCancelDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void SetText(const GUIString &theTitle, const GUIString &theText, const GUIString &theOkText, const GUIString &theCancelText);
};
typedef SmartPtr<OkCancelDialog> OkCancelDialogPtr;

}; // namespace WONAPI

#endif