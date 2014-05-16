#include "DirectConnectCtrl.h"

#include "LobbyEvent.h"
#include "ServerListCtrl.h"

#include "WONGUI/Button.h"


using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DirectConnectCtrl::DirectConnectCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DirectConnectCtrl::~DirectConnectCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DirectConnectCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mConnectionList,"ConnectionList");
		WONComponentConfig_Get(aConfig,mAddButton,"AddButton");
		WONComponentConfig_Get(aConfig,mRemoveButton,"RemoveButton");

		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DirectConnectCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ButtonPressed:
			if(theEvent->mComponent==mAddButton)
				FireEvent(new ChangeDirectConnectEvent(LobbyChangeType_Add)); 
			else if(theEvent->mComponent==mRemoveButton)
			{
				LobbyGame *aGame = mConnectionList->GetSelGame();
				if(aGame!=NULL)
					FireEvent(new ChangeDirectConnectEvent(LobbyChangeType_Delete,aGame));
			}
			else				
				break;

			return;
	}

	Container::HandleComponentEvent(theEvent);
}
