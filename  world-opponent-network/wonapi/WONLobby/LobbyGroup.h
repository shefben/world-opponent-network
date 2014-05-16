#ifndef __WON_LOBBYGROUP_H__
#define __WON_LOBBYGROUP_H__

#include "WONGUI/GUIString.h"

#include <map>

namespace WONAPI
{
class LobbyClient;
struct RoutingGroupInfo;
struct RoutingMemberInfo;
typedef SmartPtr<RoutingMemberInfo> RoutingMemberInfoPtr;
typedef std::map<unsigned short,RoutingMemberInfoPtr> RoutingMemberMap;

WON_PTR_FORWARD(LobbyClient);
WON_PTR_FORWARD(LobbyClientList);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyGroup : public RefCount
{
protected:
	GUIString mName;
	unsigned short mGroupId;
	unsigned short mCaptainId;

	int mNumUsers;
	int mMaxUsers;
	int mGroupFlags;

	LobbyClientListPtr mClientList;


	virtual ~LobbyGroup();

public:
	LobbyGroup();
	void Init(RoutingGroupInfo *theGroup, LobbyClientList *theServerClientList);
	void UpdateMembers(const RoutingMemberMap &theMap, LobbyClientList *theServerClientList);

	const GUIString& GetName() { return mName; }
	bool IsPasswordProtected();
	bool IsInviteOnly();
	bool IsAskToJoin();

	bool IsChatRoom();
	bool IsGameRoom();
	bool IsOpenChatRoom();

	unsigned short GetGroupId() { return mGroupId; }
	unsigned short GetCaptainId() { return mCaptainId; }
	int GetNumUsers() { return mNumUsers; }
	int GetMaxUsers() { return mMaxUsers; }

	void AddClient(LobbyClient *theClient, unsigned char theMemberFlags);
	LobbyClientPtr RemoveClient(unsigned short theId);
	LobbyClient* GetClient(unsigned short theId);
	LobbyClientList* GetClientList() { return mClientList; }
	LobbyClient* GetCaptain();
	LobbyClient* SetCaptain(unsigned short theId, LobbyClient** theOld); // returns new captain, theOld fills in old captain
};
typedef SmartPtr<LobbyGroup> LobbyGroupPtr;
typedef std::map<unsigned short,LobbyGroupPtr> LobbyGroupMap;

} // namespace WONAPI

#endif