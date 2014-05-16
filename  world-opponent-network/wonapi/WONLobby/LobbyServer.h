#ifndef __WON_LOBBYSERVER_H__
#define __WON_LOBBYSERVER_H__

#include "WONGUI/GUIString.h"
#include "WONSocket/IPAddr.h"

#include <string>
#include <map>
#include <list>


namespace WONAPI
{

struct DirEntity;
typedef ConstSmartPtr<DirEntity> DirEntityPtr; 
typedef std::list<DirEntityPtr> DirEntityList;

struct RoutingClientInfo;
typedef SmartPtr<RoutingClientInfo> RoutingClientInfoPtr;
typedef std::map<unsigned short, RoutingClientInfoPtr> RoutingClientMap;

struct RoutingGroupInfo;
typedef SmartPtr<RoutingGroupInfo> RoutingGroupInfoPtr;
typedef std::map<unsigned short, RoutingGroupInfoPtr> RoutingGroupMap;

WON_PTR_FORWARD(LobbyGroup);
typedef std::map<unsigned short,LobbyGroupPtr> LobbyGroupMap;

WON_PTR_FORWARD(LobbyClient);
WON_PTR_FORWARD(LobbyClientList);
WON_PTR_FORWARD(RoutingConnection);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyServer : public RefCount
{
protected:
	GUIString mName;
	typedef std::set<std::string,StringLessNoCase> LanguageSet;
	LanguageSet mLanguageSet;

	IPAddr mIPAddr;
	int mNumUsers;
	int mNumClients;

	time_t mLastGroupQueryTime;
	bool mQueryingGroups;
	LobbyGroupMap mGroupMap;

	LobbyClientListPtr mClientList;
	RoutingConnectionPtr mConnection;
	unsigned short mMyClientId;

	virtual ~LobbyServer();

public:
	LobbyServer();

	void Update(const DirEntity *theEntity);
	void UpdateClients(const RoutingClientMap &theMap);
	void UpdateGroups(const RoutingGroupMap &theMap, bool includesMembers);
	void UpdateFriends();
	void UpdateFriendTimes(); // mark last search time as now for any friend on this server

	LobbyClient* AddClient(RoutingClientInfo *theInfo);
	void AddClientToGroup(LobbyClient *theClient, unsigned char theMemberFlags, LobbyGroup *theGroup);
	LobbyClientPtr RemoveClient(unsigned short theId);
	LobbyClientPtr RemoveClientFromGroup(unsigned short theClientId, LobbyGroup *theGroup);
	LobbyGroup* AddGroup(RoutingGroupInfo *theInfo);
	LobbyGroupPtr RemoveGroup(unsigned short theId);
	void ClearGroups();

	void SetMyClientId(unsigned short theId) { mMyClientId = theId; }
	unsigned short GetMyClientId() { return mMyClientId; }
	LobbyClient* GetMyClient();

	time_t GetLastGroupQueryTime() { return mLastGroupQueryTime; }
	bool IsQueryingGroups() { return mQueryingGroups; }

	void SetLastGroupQueryTime(time_t theTime) { mLastGroupQueryTime = theTime; }
	void SetQueryingGroups(bool isQuerying) { mQueryingGroups = isQuerying; }

	bool IsConnected();
	int GetNumUsers();
	const GUIString& GetName() { return mName; }
	const IPAddr& GetIPAddr() { return mIPAddr; }
	LobbyGroup* GetGroup(int theId);
	LobbyClient* GetClient(int theId);
	LobbyClient* GetClientByWONId(unsigned long theId);
	bool SupportsLanguage(const std::string &theLanguage);

	LobbyClientList* GetClientList() { return mClientList; }
	const LobbyGroupMap& GetGroupMap() { return mGroupMap; }

	void SetConnection(RoutingConnection *theConnection);
	RoutingConnection* GetConnection() { return mConnection; }
	void KillConnection();
	void SetNumClients(unsigned short theNumClients) { mNumClients = theNumClients; }
};
typedef SmartPtr<LobbyServer> LobbyServerPtr;
typedef std::map<IPAddr,LobbyServerPtr> LobbyServerMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyServerList : public RefCount
{
protected:
	LobbyServerMap mServerMap;
	virtual ~LobbyServerList();

public:
	LobbyServerList();
	
	void Update(const DirEntityList &theEntityList);

	LobbyServer* GetServer(const IPAddr &theAddr);
	LobbyServer* GetRandomServer();

	const LobbyServerMap& GetServerMap() { return mServerMap; }
};
typedef SmartPtr<LobbyServerList> LobbyServerListPtr;


} // namespace WONAPI

#endif