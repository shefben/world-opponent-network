#include "RoomLabelCtrl.h"
#include "LobbyResource.h"
#include "LobbyClient.h"
#include "LobbyEvent.h"

#include "WONGUI/Label.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoomLabelCtrl::RoomLabelCtrl() :
	mRoomSpecFlags(0),
	mUserCt(0)
{
	mShowServer = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomLabelCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mLabel,"Label");
	  	
		CalcLabel();
		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomLabelCtrl::CalcLabel()
{
	GUIString aLabelStr;
	char      aUserCtBuf[33] = "";

	if (mRoom.empty())
	{
		aLabelStr = RoomLabelCtrl_LabelDisconnected_String;
	}
	else
	{
		if (! mPrefix.empty())
			aLabelStr.append(RoomLabelCtrl_LabelPrefix_String);

		if ((mShowServer) && (! mServer.empty()))
			aLabelStr.append(RoomLabelCtrl_LabelServer_String);

		aLabelStr.append(mRoom);

		if (mUserCt > 0)
		{
			sprintf(aUserCtBuf, "%d", mUserCt);
			aLabelStr.append(mUserCt == 1 ? RoomLabelCtrl_LabelUser_String : RoomLabelCtrl_LabelUsers_String);
		}
	}

	mLabel->SetText(StringLocalize::GetStr(aLabelStr,mPrefix,mServer,aUserCtBuf));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomLabelCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_JoinedRoom:
		{
			JoinedRoomEvent *anEvent = (JoinedRoomEvent*)theEvent;
			if(anEvent->mRoomSpec.Allows(mRoomSpecFlags))
			{
				mServer = anEvent->mServerName;
				mRoom   = anEvent->mRoomName;
				if (mUserCt >= 0)
					mUserCt = anEvent->mClientList->GetClientMap().size();
				CalcLabel();
			}
		}
		return;

		case LobbyEvent_LeftRoom:
		{
			LeftRoomEvent *anEvent = (LeftRoomEvent*)theEvent;
			if(anEvent->mRoomSpec.Allows(mRoomSpecFlags))
			{
				mServer.erase();;
				mRoom.erase();
				if (mUserCt > 0)
					mUserCt = 0;
				CalcLabel();
			}
		}
		return;

		case LobbyEvent_ClientChanged:
		{
			ClientChangedEvent *anEvent = (ClientChangedEvent*)theEvent;
			if ((mUserCt >= 0) && (anEvent->mRoomSpec.Allows(mRoomSpecFlags)))
			{
				if(anEvent->IsAdd())
					mUserCt++;
				else if((mUserCt > 0) && (anEvent->IsDelete()))
					mUserCt--;
				CalcLabel();
			}
		}
		return;

		case LobbyEvent_SetRoomSpecFlags:
			mRoomSpecFlags |= ((SetRoomSpecFlagsEvent*)theEvent)->mSpecFlags;
			return;
	}
		
	Container::HandleComponentEvent(theEvent);
}


	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoomLabelCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="PREFIX")
		mRoomLabel->SetPrefix(ReadString());
	else if(theInstruction=="GROUPTYPE")
	{
		GUIString aStr = ReadString();
		if(aStr.compareNoCase("CHAT")==0)
			mRoomLabel->SetRoomSpecFlags(LobbyRoomSpecFlag_Chat);
		else if(aStr.compareNoCase("GAME")==0)
			mRoomLabel->SetRoomSpecFlags(LobbyRoomSpecFlag_Game);
		else
			throw ConfigObjectException("Invalid Group Type: " + (std::string)aStr);
	}
	else if(theInstruction=="SHOWUSERCOUNT")
		mRoomLabel->SetShowUserCt(ReadBool());
	else if(theInstruction=="SHOWSERVER")
		mRoomLabel->SetShowServer(ReadBool());
	else
		return ContainerConfig::HandleInstruction(theInstruction);
	
	return true;
}

