#include "LobbyGroup.h"

#include "LobbyClient.h"

#include "WONRouting/RoutingTypes.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGroup::LobbyGroup()
{
	mNumUsers = 0;
	mMaxUsers = 0;
	mGroupFlags = 0;
	mGroupId = RoutingId_Invalid;
	mCaptainId = RoutingId_Invalid;

	mClientList = new LobbyClientList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGroup::~LobbyGroup()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGroup::Init(RoutingGroupInfo *theGroup, LobbyClientList *theServerClientList)
{
	mGroupId = theGroup->mId;
	mName = theGroup->mName;
	mNumUsers = theGroup->mMemberCount;
	mGroupFlags = theGroup->mFlags;
	mCaptainId = theGroup->mCaptainId;
	mMaxUsers = theGroup->mMaxPlayers;
	if(theServerClientList!=NULL)
		UpdateMembers(theGroup->mMemberMap, theServerClientList);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGroup::UpdateMembers(const RoutingMemberMap &theMap, LobbyClientList *theServerClientList)
{
	mClientList->Clear();
	RoutingMemberMap::const_iterator anItr = theMap.begin();
	while(anItr!=theMap.end())
	{
		LobbyClient *aClient = theServerClientList->GetClient(anItr->first);
		if(aClient!=NULL)
		{
			RoutingMemberInfo *aMember = anItr->second;
			aClient->SetMemberFlags(aMember->mFlags,IsGameRoom());
			aClient->SetIsCaptain(aClient->GetClientId()==mCaptainId,IsGameRoom());

			aClient->SetGroupId(mGroupId,IsGameRoom());
			mClientList->AddClient(aClient);
		}

		++anItr;
	}

	mNumUsers = mClientList->GetNumClients();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGroup::AddClient(LobbyClient *theClient, unsigned char theMemberFlags)
{
	mClientList->AddClient(theClient);
	theClient->SetGroupId(mGroupId,IsGameRoom());
	theClient->SetMemberFlags(theMemberFlags,IsGameRoom());
	theClient->SetIsCaptain(theClient->GetClientId()==mCaptainId,IsGameRoom());

	mNumUsers = mClientList->GetNumClients();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClientPtr LobbyGroup::RemoveClient(unsigned short theId)
{
	LobbyClientPtr aClient = mClientList->RemoveClient(theId);
	mNumUsers = mClientList->GetNumClients();
	if(aClient.get()!=NULL)
		aClient->ClearGroupInfo(mGroupId,IsGameRoom());

	return aClient;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LobbyGroup::SetCaptain(unsigned short theId, LobbyClient** theOld)
{
	LobbyClient *anOldCaptain = GetCaptain();
	if(anOldCaptain!=NULL)
		anOldCaptain->SetIsCaptain(false,IsGameRoom());

	LobbyClient *aNewCaptain = NULL;
	if(theId!=RoutingId_Invalid)
	{
		aNewCaptain = GetClient(theId);
		if(aNewCaptain!=NULL)
			aNewCaptain->SetIsCaptain(true,IsGameRoom());
	}

	if(theOld!=NULL)
		*theOld = anOldCaptain;

	mCaptainId = theId;
	return aNewCaptain;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LobbyGroup::GetCaptain()
{
	if(mCaptainId!=RoutingId_Invalid)
		return GetClient(mCaptainId);
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LobbyGroup::GetClient(unsigned short theId)
{
	return mClientList->GetClient(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGroup::IsPasswordProtected()
{
	return (mGroupFlags & RoutingGroupFlag_PasswordProtected) != 0; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGroup::IsAskToJoin()
{
	return (mGroupFlags & RoutingGroupFlag_AskCaptainToJoin) != 0; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGroup::IsInviteOnly()
{
	return (mGroupFlags & RoutingGroupFlag_InviteOnly) != 0; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGroup::IsChatRoom()
{
	return (mGroupFlags & RoutingGroupFlag_IsChatRoom)?true:false; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGroup::IsGameRoom()
{
	return (mGroupFlags & RoutingGroupFlag_IsChatRoom)?false:true; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGroup::IsOpenChatRoom()
{
	return IsChatRoom() && !IsPasswordProtected() && !IsInviteOnly() && !IsAskToJoin(); 
}

