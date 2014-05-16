#ifndef __WON_CHOOSENETWORKADAPTERDIALOG_H__
#define __WON_CHOOSENETWORKADAPTERDIALOG_H__

#include "WONGUI/Dialog.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(MultiListArea);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChooseNetworkAdapterDialog : public Dialog
{
protected:
	MultiListAreaPtr mAdapterList;
	ButtonPtr mActivateButton;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	virtual ~ChooseNetworkAdapterDialog();

public:
	ChooseNetworkAdapterDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void Reset();
	long GetSelHost();
};
typedef SmartPtr<ChooseNetworkAdapterDialog> ChooseNetworkAdapterDialogPtr;

}; // namespace WONAPI

#endif