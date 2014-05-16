#ifndef __DIRECTCONNECTCTRL_H__
#define __DIRECTCONNECTCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(ServerListCtrl);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DirectConnectCtrl : public Container
{
protected:
	ServerListCtrlPtr	mConnectionList;
	ButtonPtr		mAddButton;
	ButtonPtr		mRemoveButton;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual ~DirectConnectCtrl();

public:
	DirectConnectCtrl();
	virtual void InitComponent(ComponentInit &theInit);
};
typedef SmartPtr<DirectConnectCtrl> DirectConnectCtrlPtr;



};

#endif
