#include "LobbyServer.h"

#include "LobbyClient.h"
#include "LobbyFriend.h"
#include "LobbyGroup.h"
#include "LobbyPersistentData.h"
#include "LobbyTypes.h"


#include "WONCommon/LittleEndian.h"
#include "WONDir/DirEntity.h"
#include "WONRouting/RoutingConnection.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServer::LobbyServer()
{
	mNumUsers = 0;
	mNumClients = 0;

	mLastGroupQueryTime = 0;
	mQueryingGroups = false;
	mMyClientId = RoutingId_Invalid;

	mClientList = new LobbyClientList;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyServer::SupportsLanguage(const std::string &theLanguage)
{
	return mLanguageSet.find(theLanguage)!=mLanguageSet.end();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServer::~LobbyServer()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::SetConnection(RoutingConnection *theConnection)
{
	mConnection = theConnection;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::KillConnection()
{
	if(mConnection.get()!=NULL)
	{
		UpdateFriendTimes();
		mNumClients = mClientList->GetNumClients();
		mConnection->QueueClose();
		mConnection = NULL;
	}
	mMyClientId = RoutingId_Invalid;
	mClientList->Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::Update(const DirEntity *theEntity)
{
	mName = theEntity->mDisplayName;
	mIPAddr = theEntity->GetNetAddrAsIP();

	if(!theEntity->mDataObjects.empty())
	{
		const DirDataObjectList &aList = theEntity->mDataObjects;
		DirDataObjectList::const_iterator anItr = aList.begin();
		while(anItr!=aList.end())
		{
			const DirDataObject &anObj = *anItr;

			if(anObj.mDataType=="_C" && anObj.mData->length()==2)
				mNumClients = ShortFromLittleEndian(*(unsigned short*)anObj.mData->data());				
			else if(anObj.mDataType=="_L")
			{
				mLanguageSet.clear();

				std::string aLangStr(anObj.mData->data(), anObj.mData->length());
				StringParser aParser(aLangStr.c_str());
				WONTypes::StringList aLangList;
				aParser.ReadValue(aLangList);

				for(WONTypes::StringList::iterator anItr = aLangList.begin(); anItr!=aLangList.end(); ++anItr)
					mLanguageSet.insert(*anItr);
			}

			++anItr;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::UpdateClients(const RoutingClientMap &theMap)
{
	mClientList->UpdateClients(theMap);
	mClientList->CheckPersistentData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::UpdateFriends()
{
	// Fill in friend info
	const LobbyClientMap &aMap = mClientList->GetClientMap();
	for(LobbyClientMap::const_iterator anItr = aMap.begin(); anItr!=aMap.end(); ++anItr)
	{
		LobbyClient *aClient = anItr->second;
		LobbyFriend *aFriend = LobbyPersistentData::GetFriend(aClient->GetWONUserId());
		if(aFriend!=NULL)
		{
			aClient->SetIsFriend(true);
			aFriend->NotifyOnServer(mIPAddr,mName,0);
			aFriend->UpdateFlagsFromClient(aClient);
		}
		else
			aClient->SetIsFriend(false);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::UpdateFriendTimes() // mark last search time as now for any friend on this server
{
	time_t aTime = time(NULL);

	// Fill in friend info
	const LobbyClientMap &aMap = mClientList->GetClientMap();
	for(LobbyClientMap::const_iterator anItr = aMap.begin(); anItr!=aMap.end(); ++anItr)
	{
		LobbyClient *aClient = anItr->second;
		if(!aClient->IsFriend())
			continue;

		LobbyFriend *aFriend = LobbyPersistentData::GetFriend(aClient->GetWONUserId());
		if(aFriend==NULL)
			continue;

		if(aClient->IsAnonymous())
			aFriend->NotifyLeftServer(); // clear any knowledge about anonymous friend
		else
			aFriend->SetLastLocateTime(aTime);
	}	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::UpdateGroups(const RoutingGroupMap &theMap, bool includesMembers)
{
	mNumUsers = 0;
	mNumClients = 0;
	LobbyClientList *aClientList = includesMembers?mClientList:NULL;

	LobbyGroupMap::iterator anItr1 = mGroupMap.begin();
	LobbyGroupMap::iterator anInsertPoint = mGroupMap.begin();
	RoutingGroupMap::const_iterator anItr2 = theMap.begin();
	while(anItr1!=mGroupMap.end() && anItr2!=theMap.end())
	{
		if(anItr1->first<anItr2->first)
			mGroupMap.erase(anItr1++);
		else if(anItr2->first < anItr1->first)
		{	
			LobbyGroupPtr aGroup = new LobbyGroup;
			aGroup->Init(anItr2->second, aClientList);
			anInsertPoint = mGroupMap.insert(anInsertPoint, LobbyGroupMap::value_type(aGroup->GetGroupId(),aGroup));
			if(aGroup->IsChatRoom())
				mNumUsers += aGroup->GetNumUsers();

			mNumClients += aGroup->GetNumUsers();

			++anItr2;
		}
		else 
		{
			LobbyGroupPtr aGroup = anItr1->second;
			aGroup->Init(anItr2->second, aClientList);
			if(aGroup->IsChatRoom())
				mNumUsers += aGroup->GetNumUsers();

			mNumClients += aGroup->GetNumUsers();
			++anItr1; ++anItr2;
		}
	}

	while(anItr1!=mGroupMap.end())
		mGroupMap.erase(anItr1++);

	while(anItr2!=theMap.end())
	{
		LobbyGroupPtr aGroup = new LobbyGroup;
		aGroup->Init(anItr2->second, aClientList);
		anInsertPoint =  mGroupMap.insert(anInsertPoint, LobbyGroupMap::value_type(aGroup->GetGroupId(), aGroup));
		if(aGroup->IsChatRoom())
			mNumUsers += aGroup->GetNumUsers();

		mNumClients += aGroup->GetNumUsers();
		++anItr2;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LobbyServer::AddClient(RoutingClientInfo *theInfo)
{
	LobbyClientPtr aClient = new LobbyClient;
	aClient->Init(theInfo);
	aClient->CheckPersistentData();
	mClientList->AddClient(aClient);
	return aClient;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::AddClientToGroup(LobbyClient *theClient, unsigned char theMemberFlags, LobbyGroup *theGroup)
{
	theGroup->AddClient(theClient,theMemberFlags);
	if(theGroup->IsChatRoom())
		mNumUsers++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGroup* LobbyServer::AddGroup(RoutingGroupInfo *theInfo)
{
	// Fixme: Only works if group is empty (which is the case when you get a RoutingGroupCreated message)
	LobbyGroupPtr aGroup = new LobbyGroup;
	aGroup->Init(theInfo, mClientList);
	mGroupMap[aGroup->GetGroupId()] = aGroup;
	return aGroup;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGroupPtr LobbyServer::RemoveGroup(unsigned short theId)
{
	LobbyGroupMap::iterator anItr = mGroupMap.find(theId);
	if(anItr==mGroupMap.end())
		return NULL;

	LobbyGroupPtr aGroup = anItr->second;
	mGroupMap.erase(anItr);
	return aGroup;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClientPtr LobbyServer::RemoveClient(unsigned short theId)
{
	return mClientList->RemoveClient(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClientPtr LobbyServer::RemoveClientFromGroup(unsigned short theClientId, LobbyGroup *theGroup)
{
	LobbyClientPtr aClient = theGroup->RemoveClient(theClientId);
	if(aClient.get()!=NULL && theGroup->IsChatRoom())
	{
		mNumUsers--;
		if(mNumUsers<0) // sanity check
			mNumUsers = 0;
	}

	return aClient;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LobbyServer::GetClient(int theId)
{
	return mClientList->GetClient(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LobbyServer::GetClientByWONId(unsigned long theId)
{
	return mClientList->GetClientByWONId(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LobbyServer::GetMyClient()
{
	if(mMyClientId==LobbyServerId_Invalid)
		return NULL;
	else
		return GetClient(mMyClientId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGroup* LobbyServer::GetGroup(int theId)
{
	LobbyGroupMap::iterator anItr = mGroupMap.find(theId);
	if(anItr!=mGroupMap.end())
		return anItr->second;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LobbyServer::GetNumUsers()
{
	if(IsConnected())
		return mClientList->GetNumClients();
	else
		return mNumClients;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyServer::IsConnected()
{
	return mConnection.get()!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServer::ClearGroups()
{
	mGroupMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServerList::LobbyServerList()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServerList::~LobbyServerList()
{
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyServerList::Update(const DirEntityList &theEntityList)
{
	DirEntityList::const_iterator aRoomItr = theEntityList.begin();
	while(aRoomItr!=theEntityList.end())
	{
		const DirEntity *anEntity = *aRoomItr;
		LobbyServerMap::iterator anItr = mServerMap.insert(LobbyServerMap::value_type(anEntity->GetNetAddrAsIP(),NULL)).first;
		if(anItr->second.get()==NULL)
			anItr->second = new LobbyServer;

		LobbyServer *aServer = anItr->second;
		if(!aServer->IsConnected()) // if not getting updates from the server anyway
			aServer->Update(anEntity);

		++aRoomItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServer* LobbyServerList::GetServer(const IPAddr &theAddr)
{
	LobbyServerMap::iterator anItr = mServerMap.find(theAddr);
	if(anItr==mServerMap.end())
		return NULL;
	else
		return anItr->second;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServer* LobbyServerList::GetRandomServer()
{
	if(mServerMap.empty())
		return NULL;

	int aNum = rand()%mServerMap.size();
	LobbyServerMap::iterator anItr = mServerMap.begin();
	for(int i=0; i<aNum; i++)
		++anItr;

	return anItr->second;
}

